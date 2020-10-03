/**
 * Hook module for hooking libusb 0.1
 *
 * Based on original capnhook code for windows by decafcode:
 * https://github.com/decafcode/capnhook
 */
#ifndef CAPNHOOK_USBHOOK_H
#define CAPNHOOK_USBHOOK_H

#include <usb.h>

#include "capnhook/hook/iobuf.h"

/**
 * Available operations to hook
 */
enum cnh_usbhook_irp_op {
    CNH_USBHOOK_IRP_OP_INIT = 0,
    CNH_USBHOOK_IRP_OP_FIND_BUSSES = 1,
    CNH_USBHOOK_IRP_OP_FIND_DEVICES = 2,
    CNH_USBHOOK_IRP_OP_OPEN = 3,
    CNH_USBHOOK_IRP_OP_CLOSE = 4,
    CNH_USBHOOK_IRP_OP_RESET = 5,
    CNH_USBHOOK_IRP_OP_SET_ALTINTERFACE = 6,
    CNH_USBHOOK_IRP_OP_SET_CONFIGURATION = 7,
    CNH_USBHOOK_IRP_OP_CLAIM_INTERFACE = 8,
    CNH_USBHOOK_IRP_OP_CTRL_MSG = 9
};

/**
 * I/O request packet
 */
struct cnh_usbhook_irp {
    enum cnh_usbhook_irp_op op;
    size_t next_handler;
    int find_busses_res_num_busses;
    int find_devices_res_num_devices;
    struct usb_device* open_usb_dev;
    usb_dev_handle* handle;
    int set_altinterface;
    int set_configuration;
    int claim_interface;
    int ctrl_req_type;
    int ctrl_req;
    int ctrl_value;
    int ctrl_index;
    struct cnh_iobuf ctrl_buffer;
    int ctrl_timeout;
};

/**
 * Hook function type
 */
typedef enum cnh_result (*cnh_usbhook_fn_t)(struct cnh_usbhook_irp *irp);

/**
 * Add a new hook handler to the hook module.
 * The handler added is getting called on every hooked operation.
 *
 * @param fn Pointer to hook function to add
 * @return Result/error code if hooking was successful
 */
enum cnh_result cnh_usbhook_push_handler(cnh_usbhook_fn_t fn);

/**
 * Invoke the next hooked function registered in the hook module.
 * Call this from your hook handler if you want to pass on execution to further
 * hooked calls reaching the original function at the end of the hook chain.
 *
 * @param irp I/O request package to dispatch
 * @return Result of the following function dispatching the request package
 */
enum cnh_result cnh_usbhook_invoke_next(struct cnh_usbhook_irp *irp);

#endif
