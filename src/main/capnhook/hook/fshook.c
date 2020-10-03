#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "capnhook/hook/common.h"
#include "capnhook/hook/fshook.h"
#include "capnhook/hook/lib.h"

#include "util/time.h"

/* ------------------------------------------------------------------------------------------------------------------ */
/* Real funcs typedefs */
/* ------------------------------------------------------------------------------------------------------------------ */

typedef DIR* (*cnh_fshook_opendir_t)(const char* name);
typedef int (*cnh_fshook_lxstat_t)(int version, const char* file, struct stat* buf);
typedef int (*cnh_fshook_xstat_t)(int version, const char* file, struct stat* buf);
typedef int (*cnh_fshook_rename_t)(const char* old, const char* new);
typedef int (*cnh_fshook_remove_t)(const char* pathname);
typedef int (*cnh_fshook_access_t)(const char* path, int amode);

/* ------------------------------------------------------------------------------------------------------------------ */
/* Private helpers */
/* ------------------------------------------------------------------------------------------------------------------ */

static void _cnh_fshook_init(void);

static enum cnh_result _cnh_fshook_invoke_real(struct cnh_fshook_irp *irp);
static enum cnh_result _cnh_fshook_invoke_real_opendir(struct cnh_fshook_irp *irp);
static enum cnh_result _cnh_fshook_invoke_real_lxstat(struct cnh_fshook_irp *irp);
static enum cnh_result _cnh_fshook_invoke_real_xstat(struct cnh_fshook_irp *irp);
static enum cnh_result _cnh_fshook_invoke_real_rename(struct cnh_fshook_irp *irp);
static enum cnh_result _cnh_fshook_invoke_real_remove(struct cnh_fshook_irp *irp);
static enum cnh_result _cnh_fshook_invoke_real_access(struct cnh_fshook_irp *irp);

/* ------------------------------------------------------------------------------------------------------------------ */
/* Private state */
/* ------------------------------------------------------------------------------------------------------------------ */

static cnh_fshook_opendir_t _cnh_fshook_real_opendir;
static cnh_fshook_lxstat_t _cnh_fshook_real_lxstat;
static cnh_fshook_xstat_t _cnh_fshook_real_xstat;
static cnh_fshook_rename_t _cnh_fshook_real_rename;
static cnh_fshook_remove_t _cnh_fshook_real_remove;
static cnh_fshook_access_t _cnh_fshook_real_access;

static const cnh_fshook_fn_t _cnh_fshook_real_handlers[6] = {
    [CNH_FSHOOK_IRP_OP_DIR_OPEN] = _cnh_fshook_invoke_real_opendir,
    [CNH_FSHOOK_IRP_OP_LXSTAT] = _cnh_fshook_invoke_real_lxstat,
    [CNH_FSHOOK_IRP_OP_XSTAT] = _cnh_fshook_invoke_real_xstat,
    [CNH_FSHOOK_IRP_OP_RENAME] = _cnh_fshook_invoke_real_rename,
    [CNH_FSHOOK_IRP_OP_REMOVE] = _cnh_fshook_invoke_real_remove,
    [CNH_FSHOOK_IRP_OP_ACCESS] = _cnh_fshook_invoke_real_access,
};

static atomic_int _cnh_fshook_initted = ATOMIC_VAR_INIT(0);
static atomic_int _cnh_fshook_init_in_progress = ATOMIC_VAR_INIT(0);
static pthread_mutex_t _cnh_fshook_lock;
static cnh_fshook_fn_t *_cnh_fshook_handlers;
static size_t _cnh_fshook_nhandlers;

/* ------------------------------------------------------------------------------------------------------------------ */
/* Public module functions */
/* ------------------------------------------------------------------------------------------------------------------ */

enum cnh_result cnh_fshook_push_handler(cnh_fshook_fn_t fn)
{
    cnh_fshook_fn_t *new_array;
    size_t new_size;
    enum cnh_result result;

    assert(fn != NULL);

    _cnh_fshook_init();

    pthread_mutex_lock(&_cnh_fshook_lock);

    new_size = _cnh_fshook_nhandlers + 1;
    new_array = realloc(_cnh_fshook_handlers, new_size * sizeof(cnh_fshook_fn_t));

    if (new_array != NULL) {
        _cnh_fshook_handlers = new_array;
        _cnh_fshook_handlers[_cnh_fshook_nhandlers++] = fn;
        result = CNH_RESULT_SUCCESS;
    } else {
        result = CNH_RESULT_OUT_OF_MEMORY;
    }

    pthread_mutex_unlock(&_cnh_fshook_lock);

    return result;
}

enum cnh_result cnh_fshook_invoke_next(struct cnh_fshook_irp *irp)
{
    cnh_fshook_fn_t handler;
    enum cnh_result result;

    assert(irp != NULL);
    assert(_cnh_fshook_initted > 0);

    pthread_mutex_lock(&_cnh_fshook_lock);

    assert(irp->next_handler <= _cnh_fshook_nhandlers);

    if (irp->next_handler < _cnh_fshook_nhandlers) {
        handler = _cnh_fshook_handlers[irp->next_handler];
        irp->next_handler++;
    } else {
        handler = _cnh_fshook_invoke_real;
        irp->next_handler = (size_t) -1;
    }

    pthread_mutex_unlock(&_cnh_fshook_lock);

    result = handler(irp);

    if (result != CNH_RESULT_SUCCESS) {
        irp->next_handler = (size_t) -1;
    }

    return result;
}

/* ------------------------------------------------------------------------------------------------------------------ */
/* Hooked functions */
/* ------------------------------------------------------------------------------------------------------------------ */

DIR* opendir(const char* name)
{
    struct cnh_fshook_irp irp;
    enum cnh_result result;

    /* Ensure module is initialized */
    _cnh_fshook_init();

    memset(&irp, 0, sizeof(irp));
    irp.op = CNH_FSHOOK_IRP_OP_DIR_OPEN;
    irp.opendir_name = name;
    irp.opendir_ret = NULL;

    result = cnh_fshook_invoke_next(&irp);

    if (result != CNH_RESULT_SUCCESS) {
        errno = cnh_result_to_errno(result);
        return NULL;
    }

    errno = 0;

    return irp.opendir_ret;
}

int __lxstat(int version, const char* file, struct stat* buf)
{
    struct cnh_fshook_irp irp;
    enum cnh_result result;

    /* Ensure module is initialized */
    _cnh_fshook_init();

    memset(&irp, 0, sizeof(irp));
    irp.op = CNH_FSHOOK_IRP_OP_LXSTAT;
    irp.xstat_version = version;
    irp.xstat_file = file;
    irp.xstat_buf = buf;

    result = cnh_fshook_invoke_next(&irp);

    if (result != CNH_RESULT_SUCCESS) {
        errno = cnh_result_to_errno(result);
        return -1;
    }

    errno = 0;

    return 0;
}

int __xstat(int version, const char* file, struct stat* buf)
{
    struct cnh_fshook_irp irp;
    enum cnh_result result;

    /* Ensure module is initialized */
    _cnh_fshook_init();

    memset(&irp, 0, sizeof(irp));
    irp.op = CNH_FSHOOK_IRP_OP_XSTAT;
    irp.xstat_version = version;
    irp.xstat_file = file;
    irp.xstat_buf = buf;

    result = cnh_fshook_invoke_next(&irp);

    if (result != CNH_RESULT_SUCCESS) {
        errno = cnh_result_to_errno(result);
        return -1;
    }

    errno = 0;

    return 0;
}

int rename(const char* old, const char* new)
{
    struct cnh_fshook_irp irp;
    enum cnh_result result;

    /* Ensure module is initialized */
    _cnh_fshook_init();

    if (old == NULL || new == NULL) {
        errno = cnh_result_to_errno(CNH_RESULT_INVALID_PARAMETER);
        return -1;
    }

    memset(&irp, 0, sizeof(irp));
    irp.op = CNH_FSHOOK_IRP_OP_RENAME;
    irp.rename_old = old;
    irp.rename_new = new;

    result = cnh_fshook_invoke_next(&irp);

    if (result != CNH_RESULT_SUCCESS) {
        errno = cnh_result_to_errno(result);
        return -1;
    }

    errno = 0;

    return 0;
}

int remove(const char* pathname)
{
    struct cnh_fshook_irp irp;
    enum cnh_result result;

    /* Ensure module is initialized */
    _cnh_fshook_init();

    if (pathname == NULL) {
        errno = cnh_result_to_errno(CNH_RESULT_INVALID_PARAMETER);
        return -1;
    }

    memset(&irp, 0, sizeof(irp));
    irp.op = CNH_FSHOOK_IRP_OP_REMOVE;
    irp.remove_pathname = pathname;

    result = cnh_fshook_invoke_next(&irp);

    if (result != CNH_RESULT_SUCCESS) {
        errno = cnh_result_to_errno(result);
        return -1;
    }

    errno = 0;

    return 0;
}

int access(const char* path, int amode)
{
    struct cnh_fshook_irp irp;
    enum cnh_result result;

    /* Ensure module is initialized */
    _cnh_fshook_init();

    if (path == NULL) {
        errno = cnh_result_to_errno(CNH_RESULT_INVALID_PARAMETER);
        return -1;
    }

    memset(&irp, 0, sizeof(irp));
    irp.op = CNH_FSHOOK_IRP_OP_ACCESS;
    irp.access_path = path;
    irp.access_amode = amode;

    result = cnh_fshook_invoke_next(&irp);

    if (result != CNH_RESULT_SUCCESS) {
        errno = cnh_result_to_errno(result);
        return -1;
    }

    errno = 0;

    return 0;
}

/* ------------------------------------------------------------------------------------------------------------------ */
/* Helper functions */
/* ------------------------------------------------------------------------------------------------------------------ */

static void _cnh_fshook_init(void)
{
    int expected;

    if (atomic_load(&_cnh_fshook_initted) > 0) {
        return;
    }

    expected = 0;

    if (!atomic_compare_exchange_strong(&_cnh_fshook_init_in_progress, &expected, 1)) {
        while (atomic_load(&_cnh_fshook_init_in_progress) > 1) {
            util_time_sleep_us(100);
        }

        return;
    }

    /* Check again to ensure the current thread yielded between the init'd check and setting init in progress */
    if (atomic_load(&_cnh_fshook_initted) > 0) {
        /* Revert init in progress, because nothing to do anymore */
        atomic_store(&_cnh_fshook_init_in_progress, 0);
        return;
    }

    _cnh_fshook_real_opendir = (cnh_fshook_opendir_t) cnh_lib_get_func_addr("opendir");
    _cnh_fshook_real_lxstat = (cnh_fshook_lxstat_t) cnh_lib_get_func_addr("__lxstat");
    _cnh_fshook_real_xstat = (cnh_fshook_xstat_t) cnh_lib_get_func_addr("__xstat");
    _cnh_fshook_real_rename = (cnh_fshook_rename_t) cnh_lib_get_func_addr("rename");
    _cnh_fshook_real_remove = (cnh_fshook_remove_t) cnh_lib_get_func_addr("remove");
    _cnh_fshook_real_access = (cnh_fshook_access_t) cnh_lib_get_func_addr("access");

    pthread_mutex_init(&_cnh_fshook_lock, NULL);

    atomic_store(&_cnh_fshook_initted, 1);
    atomic_store(&_cnh_fshook_init_in_progress, 0);
}

static enum cnh_result _cnh_fshook_invoke_real(struct cnh_fshook_irp *irp)
{
    cnh_fshook_fn_t handler;

    assert(irp != NULL);
    assert(irp->op < _countof(_cnh_fshook_real_handlers));

    handler = _cnh_fshook_real_handlers[irp->op];

    assert(handler != NULL);

    return handler(irp);
}

static enum cnh_result _cnh_fshook_invoke_real_opendir(struct cnh_fshook_irp *irp)
{
    DIR* dir;

    assert(irp != NULL);

    dir = _cnh_fshook_real_opendir(irp->opendir_name);

    if (dir == NULL) {
        return cnh_errno_to_result(errno);
    }

    irp->opendir_ret = dir;

    return CNH_RESULT_SUCCESS;
}

static enum cnh_result _cnh_fshook_invoke_real_lxstat(struct cnh_fshook_irp *irp)
{
    int res;

    assert(irp != NULL);

    res = _cnh_fshook_real_lxstat(irp->xstat_version, irp->xstat_file, irp->xstat_buf);

    if (res != 0) {
        return cnh_errno_to_result(errno);
    }

    return CNH_RESULT_SUCCESS;
}

static enum cnh_result _cnh_fshook_invoke_real_xstat(struct cnh_fshook_irp *irp)
{
    int res;

    assert(irp != NULL);

    res = _cnh_fshook_real_xstat(irp->xstat_version, irp->xstat_file, irp->xstat_buf);

    if (res != 0) {
        return cnh_errno_to_result(errno);
    }

    return CNH_RESULT_SUCCESS;
}

static enum cnh_result _cnh_fshook_invoke_real_rename(struct cnh_fshook_irp *irp)
{
    int res;

    assert(irp != NULL);

    res = _cnh_fshook_real_rename(irp->rename_old, irp->rename_new);

    if (res != 0) {
        return cnh_errno_to_result(errno);
    }

    return CNH_RESULT_SUCCESS;
}

static enum cnh_result _cnh_fshook_invoke_real_remove(struct cnh_fshook_irp *irp)
{
    int res;

    assert(irp != NULL);

    res = _cnh_fshook_real_remove(irp->remove_pathname);

    if (res != 0) {
        return cnh_errno_to_result(errno);
    }

    return CNH_RESULT_SUCCESS;
}

static enum cnh_result _cnh_fshook_invoke_real_access(struct cnh_fshook_irp *irp)
{
    int res;

    assert(irp != NULL);

    res = _cnh_fshook_real_access(irp->access_path, irp->access_amode);

    if (res != 0) {
        return cnh_errno_to_result(errno);
    }

    return CNH_RESULT_SUCCESS;
}