#include "capnhook/hook/usbhook.h"

#include "io/piuio/defs.h"

#include "util/log.h"

#define PATCH_PIUIO_KHACK_CTRL_REQ_TYPE 0
#define PATCH_PIUIO_KHACK_CTRL_REQ 0
#define PATCH_PIUIO_KHACK_TIMEOUT 10011
#define PATCH_PIUIO_KHACK_BUFFER_SIZE PIUIO_DRV_BUFFER_SIZE * 4

static enum cnh_result patch_piuio_khack_usbhook(struct cnh_usbhook_irp *irp);

static usb_dev_handle* _patch_piuio_khack_usb_handle;

void patch_piuio_khack_init()
{
    cnh_usbhook_push_handler(patch_piuio_khack_usbhook);
    log_info("Initialized");
}

static enum cnh_result patch_piuio_khack_usbhook(struct cnh_usbhook_irp *irp)
{
    enum cnh_result result;
    uint8_t out_buffer[PATCH_PIUIO_KHACK_BUFFER_SIZE];
    uint8_t in_buffer[PATCH_PIUIO_KHACK_BUFFER_SIZE];

    if (irp->op != CNH_USBHOOK_IRP_OP_OPEN && _patch_piuio_khack_usb_handle != irp->handle) {
        return cnh_usbhook_invoke_next(irp);
    }

    switch (irp->op) {
        case CNH_USBHOOK_IRP_OP_OPEN:
        {
            if (irp->open_usb_dev->descriptor.idVendor == PIUIO_DRV_VID &&
                    irp->open_usb_dev->descriptor.idProduct == PIUIO_DRV_PID) {

                if (_patch_piuio_khack_usb_handle) {
                    log_warn("Already detected PIUIO previously, ignoring re-detection");

                    irp->handle = _patch_piuio_khack_usb_handle;
                    result = CNH_RESULT_SUCCESS;
                } else {
                    log_info("Detected PIUIO");

                    result = cnh_usbhook_invoke_next(irp);

                    if (result == CNH_RESULT_SUCCESS) {
                        _patch_piuio_khack_usb_handle = irp->handle;
                    }
                }
            } else {
                result = cnh_usbhook_invoke_next(irp);
            }

            break;
        }

        case CNH_USBHOOK_IRP_OP_CTRL_MSG:
        {
            // Identifiers for custom kernel hack introduced by some update in ITG 2 back in the days
            if (irp->ctrl_req_type == PATCH_PIUIO_KHACK_CTRL_REQ_TYPE &&
                    irp->ctrl_req == PATCH_PIUIO_KHACK_CTRL_REQ &&
                    irp->ctrl_timeout == PATCH_PIUIO_KHACK_TIMEOUT) {

                // Hack combines the four calls for each sensor into a single call
                if (irp->ctrl_buffer.nbytes != PATCH_PIUIO_KHACK_BUFFER_SIZE) {
                    log_error("Invalid buffer size for kernel hack: %d", irp->ctrl_buffer.nbytes);
                    return CNH_RESULT_INVALID_PARAMETER;
                }

                memcpy(out_buffer, irp->ctrl_buffer.bytes, irp->ctrl_buffer.nbytes);

                // Split the hack into single calls
                for (uint8_t i = 0; i < 4; i++) {
                    irp->ctrl_req_type = PIUIO_DRV_USB_CTRL_TYPE_OUT;
                    irp->ctrl_req = PIUIO_DRV_USB_CTRL_REQUEST;
                    irp->ctrl_timeout = PIUIO_DRV_USB_REQ_TIMEOUT;
                    irp->ctrl_buffer.pos = 0;
                    irp->ctrl_buffer.nbytes = PIUIO_DRV_BUFFER_SIZE;

                    memcpy(irp->ctrl_buffer.bytes, &out_buffer[PIUIO_DRV_BUFFER_SIZE * i], PIUIO_DRV_BUFFER_SIZE);

                    result = cnh_usbhook_invoke_next_reset_advance(irp);

                    if (result != CNH_RESULT_SUCCESS) {
                        return result;
                    }

                    irp->ctrl_req_type = PIUIO_DRV_USB_CTRL_TYPE_IN;
                    irp->ctrl_req = PIUIO_DRV_USB_CTRL_REQUEST;
                    irp->ctrl_timeout = PIUIO_DRV_USB_REQ_TIMEOUT;
                    irp->ctrl_buffer.pos = 0;
                    irp->ctrl_buffer.nbytes = PIUIO_DRV_BUFFER_SIZE;

                    result = cnh_usbhook_invoke_next_reset_advance(irp);

                    if (result != CNH_RESULT_SUCCESS) {
                        return result;
                    }

                    memcpy(&in_buffer[PIUIO_DRV_BUFFER_SIZE * i], irp->ctrl_buffer.bytes, PIUIO_DRV_BUFFER_SIZE);
                }

                irp->ctrl_req_type = PATCH_PIUIO_KHACK_CTRL_REQ_TYPE;
                irp->ctrl_req = PATCH_PIUIO_KHACK_CTRL_REQ;
                irp->ctrl_timeout = PATCH_PIUIO_KHACK_TIMEOUT;
                irp->ctrl_buffer.pos = PATCH_PIUIO_KHACK_BUFFER_SIZE;
                irp->ctrl_buffer.nbytes = PATCH_PIUIO_KHACK_BUFFER_SIZE;

                memcpy(irp->ctrl_buffer.bytes, in_buffer, PATCH_PIUIO_KHACK_BUFFER_SIZE);
            }

            break;
        }

        case CNH_USBHOOK_IRP_OP_CLOSE:
        {
            _patch_piuio_khack_usb_handle = NULL;

            result = cnh_usbhook_invoke_next(irp);

            break;
        }

        default:
            result = cnh_usbhook_invoke_next(irp);
            break;
    }

    return result;
}