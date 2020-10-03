#define LOG_MODULE "patch-mounts"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "capnhook/hook/filehook.h"

#include "util/log.h"

/* looks odd but hddd is intentional */
static const char* patch_mount_proc_mount_file = "/dev/hddd /mnt/hd";

static FILE* patch_mount_file = NULL;
static bool patch_mount_eof;

static enum cnh_result patch_hdd_check_filehook(struct cnh_filehook_irp *irp)
{
    enum cnh_result result;

    result = CNH_RESULT_SUCCESS;

    assert(irp);

    if (irp->op != CNH_FILEHOOK_IRP_OP_OPEN && irp->file != patch_mount_file) {
        return cnh_filehook_invoke_next(irp);
    }

    switch (irp->op) {
        case CNH_FILEHOOK_IRP_OP_OPEN:
        {
            if (!strcmp(irp->open_filename, "/proc/mounts")) {
                patch_mount_file = cnh_filehook_open_dummy_file_handle();
                irp->file = patch_mount_file;

                patch_mount_eof = false;

                log_debug("/proc/mounts opened: %p", irp->file);
            } else {
                return cnh_filehook_invoke_next(irp);
            }

            break;
        }

        case CNH_FILEHOOK_IRP_OP_FGETS:
        {
            size_t size;

            log_info("/proc/mounts fake read");

            size = strlen(patch_mount_proc_mount_file) + 1;

            /* +1: copy the null terminator, too */
            memcpy(&irp->read.bytes[irp->read.pos], patch_mount_proc_mount_file, size);
            irp->read.pos += size;

            patch_mount_eof = true;

            break;
        }

        case CNH_FILEHOOK_IRP_OP_SEEK:
        {
            /* Ignore seeking */

            break;
        }

        case CNH_FILEHOOK_IRP_OP_TELL:
        {
            /* Return size */
            irp->tell_offset = strlen(patch_mount_proc_mount_file) + 1;

            break;
        }

        case CNH_FILEHOOK_IRP_OP_EOF:
        {
            irp->eof = patch_mount_eof;
            break;
        }

        case CNH_FILEHOOK_IRP_OP_CLOSE:
        {
            patch_mount_file = NULL;

            /* Actually close dummy handle */
            return cnh_filehook_invoke_next(irp);
        }

        default:
            log_warn("Unhandled file op: %d", irp->op);
            break;
    }

    return result;
}

void patch_mounts_init(void)
{
    cnh_filehook_push_handler(patch_hdd_check_filehook);
    log_info("Initialized");
}