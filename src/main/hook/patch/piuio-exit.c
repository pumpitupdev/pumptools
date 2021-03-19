
#include "capnhook/hook/usbhook.h"

#include "io/piuio/defs.h"

#include "util/log.h"

static enum cnh_result patch_piuio_exit_usbhook(struct cnh_usbhook_irp *irp);

static usb_dev_handle *_patch_piuio_exit_usb_handle;

void patch_piuio_exit_init()
{
  cnh_usbhook_push_handler(patch_piuio_exit_usbhook);
  log_info("Initialized");
}

static enum cnh_result patch_piuio_exit_usbhook(struct cnh_usbhook_irp *irp)
{
  enum cnh_result result;

  if (irp->op != CNH_USBHOOK_IRP_OP_OPEN &&
      _patch_piuio_exit_usb_handle != irp->handle) {
    return cnh_usbhook_invoke_next(irp);
  }

  switch (irp->op) {
    case CNH_USBHOOK_IRP_OP_OPEN: {
      if (irp->open_usb_dev->descriptor.idVendor == PIUIO_DRV_VID &&
          irp->open_usb_dev->descriptor.idProduct == PIUIO_DRV_PID) {

        if (_patch_piuio_exit_usb_handle) {
          log_warn("Already detected PIUIO previously, ignore re-detection?");

          irp->handle = _patch_piuio_exit_usb_handle;
          result = CNH_RESULT_SUCCESS;
        } else {
          log_info("Detected PIUIO");

          result = cnh_usbhook_invoke_next(irp);

          if (result == CNH_RESULT_SUCCESS) {
            _patch_piuio_exit_usb_handle = irp->handle;
          }
        }
      } else {
        result = cnh_usbhook_invoke_next(irp);
      }

      break;
    }

    case CNH_USBHOOK_IRP_OP_CTRL_MSG: {
      result = cnh_usbhook_invoke_next(irp);

      /* Post process inputs */
      if (irp->ctrl_req_type == PIUIO_DRV_USB_CTRL_TYPE_IN &&
          irp->ctrl_req == PIUIO_DRV_USB_CTRL_REQUEST) {
        if (irp->ctrl_buffer.nbytes != PIUIO_DRV_BUFFER_SIZE) {
          log_error(
              "Invalid buffer size for ctrl in: %d", irp->ctrl_buffer.nbytes);
        } else {
          /* Service + Test button pressed */
          /* Invert data -> pull up active */
          if (((~irp->ctrl_buffer.bytes[1]) & (1 << 1)) &&
              ((~irp->ctrl_buffer.bytes[1]) & (1 << 6))) {
            log_info("Exit on service + test enabled and hit, bye");
            exit(0);
          }
        }
      }

      break;
    }

    case CNH_USBHOOK_IRP_OP_CLOSE: {
      _patch_piuio_exit_usb_handle = NULL;

      result = cnh_usbhook_invoke_next(irp);

      break;
    }

    default:
      result = cnh_usbhook_invoke_next(irp);
      break;
  }

  return result;
}