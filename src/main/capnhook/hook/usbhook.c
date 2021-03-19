#define LOG_MODULE "cnh-usbhook"

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <string.h>
#include <usb.h>

#include "capnhook/hook/common.h"
#include "capnhook/hook/lib.h"
#include "capnhook/hook/usbhook.h"

#include "util/time.h"

/* ------------------------------------------------------------------------------------------------------------------ */
/* Real funcs typedefs */
/* ------------------------------------------------------------------------------------------------------------------ */

typedef int (*cnh_usbhook_usb_init_t)(void);
typedef int (*cnh_usbhook_usb_find_devices_t)(void);
typedef int (*cnh_usbhook_usb_find_busses_t)(void);
typedef usb_dev_handle* (*cnh_usbhook_usb_open_t)(struct usb_device* dev);
typedef int (*cnh_usbhook_usb_close_t)(usb_dev_handle* dev);
typedef int (*cnh_usbhook_usb_reset_t)(usb_dev_handle* dev);
typedef int (*cnh_usbhook_usb_set_altinterface_t)(usb_dev_handle* dev, int interface);
typedef int (*cnh_usbhook_usb_set_configuration_t)(usb_dev_handle* dev, int configuration);
typedef int (*cnh_usbhook_usb_claim_interface_t)(usb_dev_handle* dev, int interface);
typedef int (*cnh_usbhook_usb_control_msg_t)(usb_dev_handle* dev, int requesttype, int request, int value, int index,
    char* bytes, int nbytes, int timeout);

/* ------------------------------------------------------------------------------------------------------------------ */
/* Private helpers */
/* ------------------------------------------------------------------------------------------------------------------ */

static void _cnh_usbhook_init(void);
static enum cnh_result _cnh_usbhook_invoke_next_reset_advance(struct cnh_usbhook_irp *irp, bool reset_next_handler_advance);

static enum cnh_result _cnh_usbhook_invoke_real(struct cnh_usbhook_irp* irp);
static enum cnh_result _cnh_usbhook_invoke_real_init(struct cnh_usbhook_irp* irp);
static enum cnh_result _cnh_usbhook_invoke_real_find_busses(struct cnh_usbhook_irp* irp);
static enum cnh_result _cnh_usbhook_invoke_real_find_devices(struct cnh_usbhook_irp* irp);
static enum cnh_result _cnh_usbhook_invoke_real_open(struct cnh_usbhook_irp* irp);
static enum cnh_result _cnh_usbhook_invoke_real_close(struct cnh_usbhook_irp* irp);
static enum cnh_result _cnh_usbhook_invoke_real_reset(struct cnh_usbhook_irp* irp);
static enum cnh_result _cnh_usbhook_invoke_real_set_altinterface(struct cnh_usbhook_irp* irp);
static enum cnh_result _cnh_usbhook_invoke_real_set_configuration(struct cnh_usbhook_irp* irp);
static enum cnh_result _cnh_usbhook_invoke_real_claim_interface(struct cnh_usbhook_irp* irp);
static enum cnh_result _cnh_usbhook_invoke_real_ctrl_msg(struct cnh_usbhook_irp* irp);

/* ------------------------------------------------------------------------------------------------------------------ */
/* Private state */
/* ------------------------------------------------------------------------------------------------------------------ */

static cnh_usbhook_usb_init_t _cnh_usbhook_real_init;
static cnh_usbhook_usb_find_devices_t _cnh_usbhook_real_find_devices;
static cnh_usbhook_usb_find_busses_t _cnh_usbhook_real_find_busses;
static cnh_usbhook_usb_open_t _cnh_usbhook_real_open;
static cnh_usbhook_usb_close_t _cnh_usbhook_real_close;
static cnh_usbhook_usb_reset_t _cnh_usbhook_real_reset;
static cnh_usbhook_usb_set_altinterface_t _cnh_usbhook_real_set_altinterface;
static cnh_usbhook_usb_set_configuration_t _cnh_usbhook_real_set_configuration;
static cnh_usbhook_usb_claim_interface_t _cnh_usbhook_real_claim_interface;
static cnh_usbhook_usb_control_msg_t _cnh_usbhook_real_control_msg;

static const cnh_usbhook_fn_t _cnh_usbhook_real_handlers[10] = {
    [CNH_USBHOOK_IRP_OP_INIT] = _cnh_usbhook_invoke_real_init,
    [CNH_USBHOOK_IRP_OP_FIND_BUSSES ]= _cnh_usbhook_invoke_real_find_busses,
    [CNH_USBHOOK_IRP_OP_FIND_DEVICES] = _cnh_usbhook_invoke_real_find_devices,
    [CNH_USBHOOK_IRP_OP_OPEN] = _cnh_usbhook_invoke_real_open,
    [CNH_USBHOOK_IRP_OP_CLOSE] = _cnh_usbhook_invoke_real_close,
    [CNH_USBHOOK_IRP_OP_RESET] = _cnh_usbhook_invoke_real_reset,
    [CNH_USBHOOK_IRP_OP_SET_ALTINTERFACE] = _cnh_usbhook_invoke_real_set_altinterface,
    [CNH_USBHOOK_IRP_OP_SET_CONFIGURATION] = _cnh_usbhook_invoke_real_set_configuration,
    [CNH_USBHOOK_IRP_OP_CLAIM_INTERFACE] = _cnh_usbhook_invoke_real_claim_interface,
    [CNH_USBHOOK_IRP_OP_CTRL_MSG] = _cnh_usbhook_invoke_real_ctrl_msg,
};

static atomic_int _cnh_usbhook_initted = ATOMIC_VAR_INIT(0);
static atomic_int _cnh_usbhook_init_in_progress = ATOMIC_VAR_INIT(0);
static pthread_mutex_t _cnh_usbhook_lock;
static cnh_usbhook_fn_t *_cnh_usbhook_handlers;
static size_t _cnh_usbhook_nhandlers;

/* ------------------------------------------------------------------------------------------------------------------ */
/* Public module functions */
/* ------------------------------------------------------------------------------------------------------------------ */

enum cnh_result cnh_usbhook_push_handler(cnh_usbhook_fn_t fn)
{
    cnh_usbhook_fn_t *new_array;
    size_t new_size;
    enum cnh_result result;

    assert(fn != NULL);

    _cnh_usbhook_init();
    pthread_mutex_lock(&_cnh_usbhook_lock);

    new_size = _cnh_usbhook_nhandlers + 1;
    new_array = realloc(_cnh_usbhook_handlers, new_size * sizeof(cnh_usbhook_fn_t));

    if (new_array != NULL) {
        _cnh_usbhook_handlers = new_array;
        _cnh_usbhook_handlers[_cnh_usbhook_nhandlers++] = fn;
        result = CNH_RESULT_SUCCESS;
    } else {
        result = CNH_RESULT_OUT_OF_MEMORY;
    }

    pthread_mutex_unlock(&_cnh_usbhook_lock);

    return result;
}

enum cnh_result cnh_usbhook_invoke_next(struct cnh_usbhook_irp *irp)
{
    return _cnh_usbhook_invoke_next_reset_advance(irp, false);
}

enum cnh_result cnh_usbhook_invoke_next_reset_advance(struct cnh_usbhook_irp *irp)
{
    return _cnh_usbhook_invoke_next_reset_advance(irp, true);
}

/* ------------------------------------------------------------------------------------------------------------------ */
/* Hooked functions */
/* ------------------------------------------------------------------------------------------------------------------ */

void usb_init(void)
{
    struct cnh_usbhook_irp irp;

    /* Ensure module is initialized */
    _cnh_usbhook_init();

    memset(&irp, 0, sizeof(irp));
    irp.op = CNH_USBHOOK_IRP_OP_INIT;

    cnh_usbhook_invoke_next(&irp);

    /* No return value, results ignored */
}

int usb_find_busses(void)
{
    struct cnh_usbhook_irp irp;

    /* Ensure module is initialized */
    _cnh_usbhook_init();

    memset(&irp, 0, sizeof(irp));
    irp.op = CNH_USBHOOK_IRP_OP_FIND_BUSSES;
    irp.find_busses_res_num_busses = 0;

    cnh_usbhook_invoke_next(&irp);

    /* No errors processed */

    return irp.find_busses_res_num_busses;
}

int usb_find_devices(void)
{
    struct cnh_usbhook_irp irp;

    /* Ensure module is initialized */
    _cnh_usbhook_init();

    memset(&irp, 0, sizeof(irp));
    irp.op = CNH_USBHOOK_IRP_OP_FIND_DEVICES;
    irp.find_devices_res_num_devices = 0;

    cnh_usbhook_invoke_next(&irp);

    /* No errors processed */

    return irp.find_devices_res_num_devices;
}

usb_dev_handle* usb_open(struct usb_device* dev)
{
    struct cnh_usbhook_irp irp;
    enum cnh_result result;

    /* Ensure module is initialized */
    _cnh_usbhook_init();

    if (dev == NULL) {
        /* No errno is set here (from libusbcompat impl) */
        return NULL;
    }

    memset(&irp, 0, sizeof(irp));
    irp.op = CNH_USBHOOK_IRP_OP_OPEN;
    irp.open_usb_dev = dev;
    irp.handle = NULL;

    result = cnh_usbhook_invoke_next(&irp);

    if (result != CNH_RESULT_SUCCESS) {
        errno = cnh_result_to_errno(result);
        return NULL;
    }

    return irp.handle;
}

int usb_close(usb_dev_handle* dev)
{
    struct cnh_usbhook_irp irp;
    enum cnh_result result;

    /* Ensure module is initialized */
    _cnh_usbhook_init();

    if (dev == NULL) {
        errno = cnh_result_to_errno(CNH_RESULT_INVALID_PARAMETER);
        /* Always return negative errno on errors */
        return -errno;
    }

    memset(&irp, 0, sizeof(irp));
    irp.op = CNH_USBHOOK_IRP_OP_CLOSE;
    irp.handle = dev;

    result = cnh_usbhook_invoke_next(&irp);

    if (result != CNH_RESULT_SUCCESS) {
        errno = cnh_result_to_errno(result);
        /* Always return negative errno on errors */
        return -errno;
    }

    return 0;
}

int usb_reset(usb_dev_handle* dev)
{
    struct cnh_usbhook_irp irp;
    enum cnh_result result;

    /* Ensure module is initialized */
    _cnh_usbhook_init();

    if (dev == NULL) {
        errno = cnh_result_to_errno(CNH_RESULT_INVALID_PARAMETER);
        /* Always return negative errno on errors */
        return -errno;
    }

    memset(&irp, 0, sizeof(irp));
    irp.op = CNH_USBHOOK_IRP_OP_RESET;
    irp.handle = dev;

    result = cnh_usbhook_invoke_next(&irp);

    if (result != CNH_RESULT_SUCCESS) {
        errno = cnh_result_to_errno(result);
        /* Always return negative errno on errors */
        return -errno;
    }

    return 0;
}

int usb_set_altinterface(usb_dev_handle* dev, int interface)
{
    struct cnh_usbhook_irp irp;
    enum cnh_result result;

    /* Ensure module is initialized */
    _cnh_usbhook_init();

    if (dev == NULL) {
        errno = cnh_result_to_errno(CNH_RESULT_INVALID_PARAMETER);
        /* Always return negative errno on errors */
        return -errno;
    }

    memset(&irp, 0, sizeof(irp));
    irp.op = CNH_USBHOOK_IRP_OP_SET_ALTINTERFACE;
    irp.handle = dev;
    irp.set_altinterface = interface;

    result = cnh_usbhook_invoke_next(&irp);

    if (result != CNH_RESULT_SUCCESS) {
        errno = cnh_result_to_errno(result);
        /* Always return negative errno on errors */
        return -errno;
    }

    return 0;
}

int usb_set_configuration(usb_dev_handle* dev, int configuration)
{
    struct cnh_usbhook_irp irp;
    enum cnh_result result;

    /* Ensure module is initialized */
    _cnh_usbhook_init();

    if (dev == NULL) {
        errno = cnh_result_to_errno(CNH_RESULT_INVALID_PARAMETER);
        /* Always return negative errno on errors */
        return -errno;
    }

    memset(&irp, 0, sizeof(irp));
    irp.op = CNH_USBHOOK_IRP_OP_SET_CONFIGURATION;
    irp.handle = dev;
    irp.set_configuration = configuration;

    result = cnh_usbhook_invoke_next(&irp);

    if (result != CNH_RESULT_SUCCESS) {
        errno = cnh_result_to_errno(result);
        /* Always return negative errno on errors */
        return -errno;
    }

    return 0;
}

int usb_claim_interface(usb_dev_handle* dev, int interface)
{
    struct cnh_usbhook_irp irp;
    enum cnh_result result;

    /* Ensure module is initialized */
    _cnh_usbhook_init();

    if (dev == NULL) {
        errno = cnh_result_to_errno(CNH_RESULT_INVALID_PARAMETER);
        /* Always return negative errno on errors */
        return -errno;
    }

    memset(&irp, 0, sizeof(irp));
    irp.op = CNH_USBHOOK_IRP_OP_CLAIM_INTERFACE;
    irp.handle = dev;
    irp.claim_interface = interface;

    result = cnh_usbhook_invoke_next(&irp);

    if (result != CNH_RESULT_SUCCESS) {
        errno = cnh_result_to_errno(result);
        /* Always return negative errno on errors */
        return -errno;
    }

    return 0;
}

int usb_control_msg(usb_dev_handle* dev, int requesttype, int request, int value, int index, char* bytes, int nbytes,
        int timeout)
{
    struct cnh_usbhook_irp irp;
    enum cnh_result result;

    /* Ensure module is initialized */
    _cnh_usbhook_init();

    if (dev == NULL || bytes == NULL) {
        errno = cnh_result_to_errno(CNH_RESULT_INVALID_PARAMETER);
        /* Always return negative errno on errors */
        return -errno;
    }

    memset(&irp, 0, sizeof(irp));
    irp.op = CNH_USBHOOK_IRP_OP_CTRL_MSG;
    irp.handle = dev;
    irp.ctrl_req_type = requesttype;
    irp.ctrl_req = request;
    irp.ctrl_value = value;
    irp.ctrl_index = index;
    irp.ctrl_buffer.bytes = (uint8_t*) bytes;
    irp.ctrl_buffer.nbytes = (size_t) nbytes;
    irp.ctrl_buffer.pos = 0;
    irp.ctrl_timeout = timeout;

    result = cnh_usbhook_invoke_next(&irp);

    if (result != CNH_RESULT_SUCCESS) {
        errno = cnh_result_to_errno(result);
        /* Always return negative errno on errors */
        return -errno;
    }

    return irp.ctrl_buffer.pos;
}

/* ------------------------------------------------------------------------------------------------------------------ */
/* Helper functions */
/* ------------------------------------------------------------------------------------------------------------------ */

static void _cnh_usbhook_init(void)
{
    int expected;

    if (atomic_load(&_cnh_usbhook_initted) > 0) {
        return;
    }

    expected = 0;

    if (!atomic_compare_exchange_strong(&_cnh_usbhook_init_in_progress, &expected, 1)) {
        while (atomic_load(&_cnh_usbhook_init_in_progress) > 1) {
            util_time_sleep_us(100);
        }

        return;
    }

    /* Check again to ensure the current thread yielded between the init'd check and setting init in progress */
    if (atomic_load(&_cnh_usbhook_initted) > 0) {
        /* Revert init in progress, because nothing to do anymore */
        atomic_store(&_cnh_usbhook_init_in_progress, 0);
        return;
    }

    _cnh_usbhook_real_init = (cnh_usbhook_usb_init_t) cnh_lib_get_func_addr("usb_init");
    _cnh_usbhook_real_find_devices = (cnh_usbhook_usb_find_devices_t) cnh_lib_get_func_addr("usb_find_devices");
    _cnh_usbhook_real_find_busses = (cnh_usbhook_usb_find_busses_t) cnh_lib_get_func_addr("usb_find_busses");
    _cnh_usbhook_real_open = (cnh_usbhook_usb_open_t) cnh_lib_get_func_addr("usb_open");
    _cnh_usbhook_real_close = (cnh_usbhook_usb_close_t) cnh_lib_get_func_addr("usb_close");
    _cnh_usbhook_real_reset = (cnh_usbhook_usb_reset_t) cnh_lib_get_func_addr("usb_reset");
    _cnh_usbhook_real_set_altinterface =
        (cnh_usbhook_usb_set_altinterface_t) cnh_lib_get_func_addr("usb_set_altinterface");
    _cnh_usbhook_real_set_configuration =
        (cnh_usbhook_usb_set_configuration_t) cnh_lib_get_func_addr("usb_set_configuration");
    _cnh_usbhook_real_claim_interface =
        (cnh_usbhook_usb_claim_interface_t) cnh_lib_get_func_addr("usb_claim_interface");
    _cnh_usbhook_real_control_msg = (cnh_usbhook_usb_control_msg_t) cnh_lib_get_func_addr("usb_control_msg");

    pthread_mutex_init(&_cnh_usbhook_lock, NULL);

    atomic_store(&_cnh_usbhook_initted, 1);
    atomic_store(&_cnh_usbhook_init_in_progress, 0);
}

static enum cnh_result _cnh_usbhook_invoke_next_reset_advance(struct cnh_usbhook_irp *irp, bool reset_next_handler_advance)
{
    cnh_usbhook_fn_t handler;
    enum cnh_result result;
    size_t cur_next_handler;

    assert(irp != NULL);
    assert(_cnh_usbhook_initted > 0);

    cur_next_handler = irp->next_handler;

    pthread_mutex_lock(&_cnh_usbhook_lock);

    assert(irp->next_handler <= _cnh_usbhook_nhandlers);

    if (irp->next_handler < _cnh_usbhook_nhandlers) {
        handler = _cnh_usbhook_handlers[irp->next_handler];
        irp->next_handler++;
    } else {
        handler = _cnh_usbhook_invoke_real;
        irp->next_handler = (size_t) -1;
    }

    pthread_mutex_unlock(&_cnh_usbhook_lock);

    result = handler(irp);

    if (result != CNH_RESULT_SUCCESS) {
        irp->next_handler = (size_t) -1;
    }

    if (reset_next_handler_advance) {
        irp->next_handler = cur_next_handler;
    }

    return result;
}

static enum cnh_result _cnh_usbhook_invoke_real(struct cnh_usbhook_irp* irp)
{
    cnh_usbhook_fn_t handler;

    assert(irp != NULL);
    assert(irp->op < _countof(_cnh_usbhook_real_handlers));

    handler = _cnh_usbhook_real_handlers[irp->op];

    assert(handler != NULL);

    return handler(irp);
}

static enum cnh_result _cnh_usbhook_invoke_real_init(struct cnh_usbhook_irp* irp)
{
    assert(irp != NULL);

    _cnh_usbhook_real_init();

    return CNH_RESULT_SUCCESS;
}

static enum cnh_result _cnh_usbhook_invoke_real_find_busses(struct cnh_usbhook_irp* irp)
{
    int res;

    assert(irp != NULL);

    res = _cnh_usbhook_real_find_busses();

    irp->find_busses_res_num_busses = res;

    return CNH_RESULT_SUCCESS;
}

static enum cnh_result _cnh_usbhook_invoke_real_find_devices(struct cnh_usbhook_irp* irp)
{
    int res;

    assert(irp != NULL);

    res = _cnh_usbhook_real_find_devices();

    irp->find_devices_res_num_devices = res;

    return CNH_RESULT_SUCCESS;
}

static enum cnh_result _cnh_usbhook_invoke_real_open(struct cnh_usbhook_irp* irp)
{
    usb_dev_handle* res;

    assert(irp != NULL);

    res = _cnh_usbhook_real_open(irp->open_usb_dev);

    if (res == NULL) {
        irp->handle = NULL;
        return cnh_errno_to_result(errno);
    }

    irp->handle = res;

    return CNH_RESULT_SUCCESS;
}

static enum cnh_result _cnh_usbhook_invoke_real_close(struct cnh_usbhook_irp* irp)
{
    int res;

    assert(irp != NULL);

    res = _cnh_usbhook_real_close(irp->handle);

    if (res < 0) {
        return cnh_errno_to_result(res);
    }

    return CNH_RESULT_SUCCESS;
}

static enum cnh_result _cnh_usbhook_invoke_real_reset(struct cnh_usbhook_irp* irp)
{
    int res;

    assert(irp != NULL);

    res = _cnh_usbhook_real_reset(irp->handle);

    if (res < 0) {
        return cnh_errno_to_result(res);
    }

    return CNH_RESULT_SUCCESS;
}

static enum cnh_result _cnh_usbhook_invoke_real_set_altinterface(struct cnh_usbhook_irp* irp)
{
    int res;

    assert(irp != NULL);

    res = _cnh_usbhook_real_set_altinterface(irp->handle, irp->set_altinterface);

    if (res < 0) {
        return cnh_errno_to_result(res);
    }

    return CNH_RESULT_SUCCESS;
}

static enum cnh_result _cnh_usbhook_invoke_real_set_configuration(struct cnh_usbhook_irp* irp)
{
    int res;

    assert(irp != NULL);

    res = _cnh_usbhook_real_set_configuration(irp->handle, irp->set_configuration);

    if (res < 0) {
        return cnh_errno_to_result(res);
    }

    return CNH_RESULT_SUCCESS;
}

static enum cnh_result _cnh_usbhook_invoke_real_claim_interface(struct cnh_usbhook_irp* irp)
{
    int res;

    assert(irp != NULL);

    res = _cnh_usbhook_real_claim_interface(irp->handle, irp->claim_interface);

    if (res < 0) {
        return cnh_errno_to_result(res);
    }

    return CNH_RESULT_SUCCESS;
}

static enum cnh_result _cnh_usbhook_invoke_real_ctrl_msg(struct cnh_usbhook_irp* irp)
{
    int res;

    assert(irp != NULL);

    res = _cnh_usbhook_real_control_msg(irp->handle, irp->ctrl_req_type, irp->ctrl_req, irp->ctrl_value,
        irp->ctrl_index, (char*) irp->ctrl_buffer.bytes, irp->ctrl_buffer.nbytes, irp->ctrl_timeout);

    irp->ctrl_buffer.pos = 0;

    if (res < 0) {
        return cnh_errno_to_result(res);
    }

    irp->ctrl_buffer.pos = (size_t) res;

    return CNH_RESULT_SUCCESS;
}