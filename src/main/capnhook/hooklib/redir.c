#include <assert.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

#include "capnhook/hook/fshook.h"
#include "capnhook/hook/filehook.h"
#include "capnhook/hook/iohook.h"

#include "util/log.h"
#include "util/str.h"
#include "util/time.h"

/* ------------------------------------------------------------------------------------------------------------------ */
/* Hooks to fshook module */
/* ------------------------------------------------------------------------------------------------------------------ */

static enum cnh_result _cnh_redir_fshook_diropen(struct cnh_fshook_irp *irp);
static enum cnh_result _cnh_redir_fshook_lxstat(struct cnh_fshook_irp *irp);
static enum cnh_result _cnh_redir_fshook_xstat(struct cnh_fshook_irp *irp);
static enum cnh_result _cnh_redir_fshook_rename(struct cnh_fshook_irp *irp);
static enum cnh_result _cnh_redir_fshook_remove(struct cnh_fshook_irp *irp);
static enum cnh_result _cnh_redir_fshook_access(struct cnh_fshook_irp *irp);

static const cnh_fshook_fn_t _cnh_redir_fshook_handlers[6] = {
    [CNH_FSHOOK_IRP_OP_DIR_OPEN] = _cnh_redir_fshook_diropen,
    [CNH_FSHOOK_IRP_OP_LXSTAT] = _cnh_redir_fshook_lxstat,
    [CNH_FSHOOK_IRP_OP_XSTAT] = _cnh_redir_fshook_xstat,
    [CNH_FSHOOK_IRP_OP_RENAME] = _cnh_redir_fshook_rename,
    [CNH_FSHOOK_IRP_OP_REMOVE] = _cnh_redir_fshook_remove,
    [CNH_FSHOOK_IRP_OP_ACCESS] = _cnh_redir_fshook_access,
};

/* ------------------------------------------------------------------------------------------------------------------ */
/* Hooks to filehook module */
/* ------------------------------------------------------------------------------------------------------------------ */

static enum cnh_result _cnh_redir_filehook_default(struct cnh_filehook_irp *irp);
static enum cnh_result _cnh_redir_filehook_fopen(struct cnh_filehook_irp *irp);

static const cnh_filehook_fn_t _cnh_redir_filehook_handlers[8] = {
    [CNH_FILEHOOK_IRP_OP_OPEN] = _cnh_redir_filehook_fopen,
    [CNH_FILEHOOK_IRP_OP_CLOSE] = _cnh_redir_filehook_default,
    [CNH_FILEHOOK_IRP_OP_READ] = _cnh_redir_filehook_default,
    [CNH_FILEHOOK_IRP_OP_WRITE] = _cnh_redir_filehook_default,
    [CNH_FILEHOOK_IRP_OP_FGETS] = _cnh_redir_filehook_default,
    [CNH_FILEHOOK_IRP_OP_SEEK] = _cnh_redir_filehook_default,
    [CNH_FILEHOOK_IRP_OP_TELL] = _cnh_redir_filehook_default,
    [CNH_FILEHOOK_IRP_OP_EOF] = _cnh_redir_filehook_default,
};

/* ------------------------------------------------------------------------------------------------------------------ */
/* Hooks to iohook module */
/* ------------------------------------------------------------------------------------------------------------------ */

static enum cnh_result _cnh_redir_iohook_default(struct cnh_iohook_irp *irp);
static enum cnh_result _cnh_redir_iohook_open(struct cnh_iohook_irp *irp);

static const cnh_iohook_fn_t _cnh_redir_iohook_handlers[7] = {
    [CNH_IOHOOK_IRP_OP_OPEN] = _cnh_redir_iohook_open,
    [CNH_IOHOOK_IRP_OP_FDOPEN] = _cnh_redir_iohook_default,
    [CNH_IOHOOK_IRP_OP_CLOSE] = _cnh_redir_iohook_default,
    [CNH_IOHOOK_IRP_OP_READ] = _cnh_redir_iohook_default,
    [CNH_IOHOOK_IRP_OP_WRITE] = _cnh_redir_iohook_default,
    [CNH_IOHOOK_IRP_OP_SEEK] = _cnh_redir_iohook_default,
    [CNH_IOHOOK_IRP_OP_IOCTL] = _cnh_redir_iohook_default,
};

/* ------------------------------------------------------------------------------------------------------------------ */
/* Private state */
/* ------------------------------------------------------------------------------------------------------------------ */

struct cnh_redir_redir {
    char* src;
    char* dest;
};

static atomic_int _cnh_redir_initted = ATOMIC_VAR_INIT(0);
static atomic_int _cnh_redir_init_in_progress = ATOMIC_VAR_INIT(0);
static pthread_mutex_t _cnh_redir_lock;
static struct cnh_redir_redir* _cnh_redir_redirs;
static size_t _cnh_redir_nredirs;

static void _cnh_redir_init();
static char* _cnh_redir_check(const char* src);

/* ------------------------------------------------------------------------------------------------------------------ */
/* Public module functions */
/* ------------------------------------------------------------------------------------------------------------------ */

void cnh_redir_add(const char* src, const char* dest)
{
    struct cnh_redir_redir *new_array;
    size_t new_size;

    assert(src != NULL);
    assert(dest != NULL);

    /* Ensure module is initialized */
    _cnh_redir_init();

    pthread_mutex_lock(&_cnh_redir_lock);

    new_size = _cnh_redir_nredirs + 1;
    new_array = realloc(_cnh_redir_redirs, new_size * sizeof(struct cnh_redir_redir));

    if (new_array != NULL) {
        _cnh_redir_redirs = new_array;
    }

    _cnh_redir_redirs[_cnh_redir_nredirs].src = util_str_dup(src);
    _cnh_redir_redirs[_cnh_redir_nredirs].dest = util_str_dup(dest);

    _cnh_redir_nredirs++;

    pthread_mutex_unlock(&_cnh_redir_lock);

    log_info("Added path redirect %s -> %s", src, dest);
}

/* ------------------------------------------------------------------------------------------------------------------ */
/* Hooks to fshook module */
/* ------------------------------------------------------------------------------------------------------------------ */

enum cnh_result cnh_redir_fshook(struct cnh_fshook_irp* irp)
{
    cnh_fshook_fn_t handler;

    handler = _cnh_redir_fshook_handlers[irp->op];

    assert(handler != NULL);

    return handler(irp);
}

static enum cnh_result _cnh_redir_fshook_diropen(struct cnh_fshook_irp *irp)
{
    char* res;

    /* Ensure module is initialized */
    _cnh_redir_init();

    res = _cnh_redir_check(irp->opendir_name);

    if (res != NULL) {
        irp->opendir_name = res;
    }

    return cnh_fshook_invoke_next(irp);
}

static enum cnh_result _cnh_redir_fshook_lxstat(struct cnh_fshook_irp *irp)
{
    char* res;

    /* Ensure module is initialized */
    _cnh_redir_init();

    res = _cnh_redir_check(irp->xstat_file);

    if (res != NULL) {
        irp->xstat_file = res;
    }

    return cnh_fshook_invoke_next(irp);
}

static enum cnh_result _cnh_redir_fshook_xstat(struct cnh_fshook_irp *irp)
{
    char* res;

    /* Ensure module is initialized */
    _cnh_redir_init();

    res = _cnh_redir_check(irp->xstat_file);

    if (res != NULL) {
        irp->xstat_file = res;
    }

    return cnh_fshook_invoke_next(irp);
}

static enum cnh_result _cnh_redir_fshook_rename(struct cnh_fshook_irp *irp)
{
    char* res_old;
    char* res_new;

    /* Ensure module is initialized */
    _cnh_redir_init();

    res_old = _cnh_redir_check(irp->rename_old);
    res_new = _cnh_redir_check(irp->rename_old);

    if (res_old != NULL) {
        irp->rename_old = res_old;
    }

    if (res_new != NULL) {
        irp->rename_new = res_new;
    }

    return cnh_fshook_invoke_next(irp);
}

static enum cnh_result _cnh_redir_fshook_remove(struct cnh_fshook_irp *irp)
{
    char* res;

    /* Ensure module is initialized */
    _cnh_redir_init();

    res = _cnh_redir_check(irp->remove_pathname);

    if (res != NULL) {
        irp->remove_pathname = res;
    }

    return cnh_fshook_invoke_next(irp);
}

static enum cnh_result _cnh_redir_fshook_access(struct cnh_fshook_irp *irp)
{
    char* res;

    /* Ensure module is initialized */
    _cnh_redir_init();

    res = _cnh_redir_check(irp->access_path);

    if (res != NULL) {
        irp->access_path = res;
    }

    return cnh_fshook_invoke_next(irp);
}

/* ------------------------------------------------------------------------------------------------------------------ */
/* Hooks to iohook module */
/* ------------------------------------------------------------------------------------------------------------------ */

enum cnh_result cnh_redir_iohook(struct cnh_iohook_irp* irp)
{
    cnh_iohook_fn_t handler;

    handler = _cnh_redir_iohook_handlers[irp->op];

    assert(handler != NULL);

    return handler(irp);
}

static enum cnh_result _cnh_redir_iohook_default(struct cnh_iohook_irp *irp)
{
    return cnh_iohook_invoke_next(irp);
}

static enum cnh_result _cnh_redir_iohook_open(struct cnh_iohook_irp *irp)
{
    char* res;

    /* Ensure module is initialized */
    _cnh_redir_init();

    res = _cnh_redir_check(irp->open_filename);

    if (res != NULL) {
        irp->open_filename = res;
    }

    return cnh_iohook_invoke_next(irp);
}

/* ------------------------------------------------------------------------------------------------------------------ */
/* Hooks to filehook module */
/* ------------------------------------------------------------------------------------------------------------------ */

enum cnh_result cnh_redir_filehook(struct cnh_filehook_irp* irp)
{
    cnh_filehook_fn_t handler;

    handler = _cnh_redir_filehook_handlers[irp->op];

    assert(handler != NULL);

    return handler(irp);
}

static enum cnh_result _cnh_redir_filehook_default(struct cnh_filehook_irp *irp)
{
    return cnh_filehook_invoke_next(irp);
}

static enum cnh_result _cnh_redir_filehook_fopen(struct cnh_filehook_irp *irp)
{
    char* res;

    /* Ensure module is initialized */
    _cnh_redir_init();

    res = _cnh_redir_check(irp->open_filename);

    if (res != NULL) {
        irp->open_filename = res;
    }

    return cnh_filehook_invoke_next(irp);
}

/* ------------------------------------------------------------------------------------------------------------------ */
/* Helper functions */
/* ------------------------------------------------------------------------------------------------------------------ */

static void _cnh_redir_init()
{
    int expected;

    if (atomic_load(&_cnh_redir_initted) > 0) {
        return;
    }

    expected = 0;

    if (!atomic_compare_exchange_strong(&_cnh_redir_init_in_progress, &expected, 1)) {
        while (atomic_load(&_cnh_redir_init_in_progress) > 1) {
            util_time_sleep_us(100);
        }

        return;
    }

    /* Check again to ensure the current thread yielded between the init'd check and setting init in progress */
    if (atomic_load(&_cnh_redir_initted) > 0) {
        /* Revert init in progress, because nothing to do anymore */
        atomic_store(&_cnh_redir_init_in_progress, 0);
        return;
    }

    pthread_mutex_init(&_cnh_redir_lock, NULL);

    atomic_store(&_cnh_redir_initted, 1);
    atomic_store(&_cnh_redir_init_in_progress, 0);
}

static char* _cnh_redir_check(const char* src)
{
    const char* redir_src;
    const char* redir_dest;
    char* res;

    res = NULL;

    pthread_mutex_lock(&_cnh_redir_lock);

    for (size_t i = 0; i < _cnh_redir_nredirs; i++) {
        redir_src = _cnh_redir_redirs[i].src;
        redir_dest = _cnh_redir_redirs[i].dest;

        if (strncmp(src, redir_src, strlen(redir_src)) == 0) {
            // FIXME memory leak
            res = util_str_merge(redir_dest, &src[strlen(redir_src)]);
            log_debug("Redirect %s -> %s", src, res);
            break;
        }
    }

    pthread_mutex_unlock(&_cnh_redir_lock);

    return res;
}