#define LOG_MODULE "patch-microdog34"

#define USBDOG_ENDPOINT "/dev/usbdog"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "capnhook/hook/iohook.h"

#include "sec/microdog34/microdog34.h"

#include "util/log.h"

static int _patch_microdog34_fd = -1;

enum cnh_result patch_microdog34_iohook(struct cnh_iohook_irp *irp);

void patch_microdog34_init(const uint8_t* key_data, size_t len)
{
    sec_microdog34_init(key_data, len);

    cnh_iohook_push_handler(patch_microdog34_iohook);
    log_info("Initialized");
}

enum cnh_result patch_microdog34_iohook(struct cnh_iohook_irp *irp)
{
    if (irp->op != CNH_IOHOOK_IRP_OP_OPEN && irp->fd != _patch_microdog34_fd) {
        return cnh_iohook_invoke_next(irp);
    }

    switch (irp->op) {
        case CNH_IOHOOK_IRP_OP_OPEN:
        {
            if (!strcmp(irp->open_filename, USBDOG_ENDPOINT)) {
                _patch_microdog34_fd = cnh_iohook_open_dummy_fd();
                irp->fd = _patch_microdog34_fd;
                return CNH_RESULT_SUCCESS;
            }

            return cnh_iohook_invoke_next(irp);
        }

        case CNH_IOHOOK_IRP_OP_CLOSE:
        {
            assert(irp->fd);

            cnh_iohook_close_dummy_fd(irp->fd);
            _patch_microdog34_fd = -1;

            return CNH_RESULT_SUCCESS;
        }

        case CNH_IOHOOK_IRP_OP_READ:
        {
            /* Just stub and return success */
            return CNH_RESULT_SUCCESS;
        }

        case CNH_IOHOOK_IRP_OP_IOCTL:
        {
           irp->ioctl.pos = (size_t) sec_microdog34_process(irp->ioctl_req, irp->ioctl.bytes);

            return CNH_RESULT_SUCCESS;
        }

        default:
            log_die("Unhandled operation: %d", irp->op);
            return CNH_RESULT_OTHER_ERROR;
    }
}