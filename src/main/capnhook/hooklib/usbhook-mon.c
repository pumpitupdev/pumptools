#define LOG_MODULE "cnh-usbhook-mon"

#include "capnhook/hooklib/usbhook-mon.h"

#include "util/log.h"

static const char* _cnh_usbhook_mon_irp_op_str[] = {
    "init",
    "find_busses",
    "find_devices",
    "open",
    "close",
    "reset",
    "set_altinterface",
    "set_configuration",
    "claim_interface",
    "ctrl_msg"
};

enum cnh_result cnh_usbhook_mon(struct cnh_usbhook_irp *irp)
{
    enum cnh_result result;

    log_debug("[before][%s] next_handler %d, find_busses_res_num_busses %d, find_devices_res_num_devices %d, "
        "open_usb_dev %p, handle %p, set_altinterface %d, set_configuration %d, claim_interface %d, ctrl_req_type %d, "
        "ctrl_req %d, ctrl_value %d, ctrl_index %d, ctrl_buffer(bytes %p, nbytes %d, pos %d), ctrl_timeout %d",
        _cnh_usbhook_mon_irp_op_str[irp->op], irp->next_handler, irp->find_busses_res_num_busses,
        irp->find_devices_res_num_devices, irp->open_usb_dev, irp->handle, irp->set_altinterface,
        irp->set_configuration, irp->claim_interface, irp->ctrl_req_type, irp->ctrl_req, irp->ctrl_value,
        irp->ctrl_index, irp->ctrl_buffer.bytes, irp->ctrl_buffer.nbytes, irp->ctrl_buffer.pos, irp->ctrl_timeout);

    result = cnh_usbhook_invoke_next(irp);

    if (result != CNH_RESULT_SUCCESS) {
        log_error("[after][%s] result %d, next_handler %d, find_busses_res_num_busses %d, "
            "find_devices_res_num_devices %d, open_usb_dev %p, handle %p, set_altinterface %d, set_configuration %d, "
            "claim_interface %d, ctrl_req_type %d, ctrl_req %d, ctrl_value %d, ctrl_index %d, ctrl_buffer(bytes %p, "
            "nbytes %d, pos %d), ctrl_timeout %d", _cnh_usbhook_mon_irp_op_str[irp->op], result, irp->next_handler,
            irp->find_busses_res_num_busses, irp->find_devices_res_num_devices, irp->open_usb_dev, irp->handle,
            irp->set_altinterface, irp->set_configuration, irp->claim_interface, irp->ctrl_req_type, irp->ctrl_req,
            irp->ctrl_value, irp->ctrl_index, irp->ctrl_buffer.bytes, irp->ctrl_buffer.nbytes, irp->ctrl_buffer.pos,
            irp->ctrl_timeout);
    } else {
        log_debug("[after][%s] result %d, next_handler %d, find_busses_res_num_busses %d, "
            "find_devices_res_num_devices %d, open_usb_dev %p, handle %p, set_altinterface %d, set_configuration %d, "
            "claim_interface %d, ctrl_req_type %d, ctrl_req %d, ctrl_value %d, ctrl_index %d, ctrl_buffer(bytes %p, "
            "nbytes %d, pos %d), ctrl_timeout %d", _cnh_usbhook_mon_irp_op_str[irp->op], result, irp->next_handler,
            irp->find_busses_res_num_busses, irp->find_devices_res_num_devices, irp->open_usb_dev, irp->handle,
            irp->set_altinterface, irp->set_configuration, irp->claim_interface, irp->ctrl_req_type, irp->ctrl_req,
            irp->ctrl_value, irp->ctrl_index, irp->ctrl_buffer.bytes, irp->ctrl_buffer.nbytes, irp->ctrl_buffer.pos,
            irp->ctrl_timeout);
    }

    return result;
}