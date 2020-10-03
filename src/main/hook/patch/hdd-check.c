#define LOG_MODULE "patch-hdd-check"

#include <assert.h>
#include <linux/hdreg.h>
#include <pthread.h>
#include <string.h>

#include "capnhook/hook/filehook.h"
#include "capnhook/hook/iohook.h"

#include "util/log.h"

static const uint8_t* patch_hdd_check_boot_area;

static int patch_hdd_check_fd = -1;
static FILE* patch_hdd_check_file = NULL;
static size_t patch_hdd_check_boot_area_size;
static size_t patch_hdd_check_boot_area_fp;

static enum cnh_result patch_hdd_check_filehook(struct cnh_filehook_irp *irp)
{
    enum cnh_result result;

    result = CNH_RESULT_SUCCESS;

    assert(irp);

    if (irp->op != CNH_FILEHOOK_IRP_OP_OPEN && (!patch_hdd_check_file || irp->file != patch_hdd_check_file)) {
        return cnh_filehook_invoke_next(irp);
    }

    switch (irp->op) {
        case CNH_FILEHOOK_IRP_OP_OPEN:
        {
            if (!strcmp(irp->open_filename, "/dev/hdd")) {
                patch_hdd_check_file = cnh_filehook_open_dummy_file_handle();
                irp->file = patch_hdd_check_file;

                log_debug("/dev/hdd fopened: %p", patch_hdd_check_file);
            } else {
                return cnh_filehook_invoke_next(irp);
            }

            break;
        }

        case CNH_FILEHOOK_IRP_OP_READ:
        {
            uint32_t len;

            log_debug("fread, pos 0x%X, bytes %d", patch_hdd_check_boot_area_fp, irp->read.nbytes);

            /* read up to eof, only */
            if (patch_hdd_check_boot_area_fp + irp->read.nbytes < patch_hdd_check_boot_area_size) {
                len = irp->read.nbytes;
            } else {
                len = patch_hdd_check_boot_area_size - patch_hdd_check_boot_area_fp;
            }

            memcpy(irp->read.bytes, patch_hdd_check_boot_area + patch_hdd_check_boot_area_fp, len);

            patch_hdd_check_boot_area_fp += len;
            irp->read.pos = len;

            break;
        }

        case CNH_FILEHOOK_IRP_OP_WRITE:
        {
            // Apparently, Prime starts writing to /dev/hdd, stub
            log_debug("fwrite");

            break;
        }

        case CNH_FILEHOOK_IRP_OP_SEEK:
        {
            /* TODO this is not verified to work, yet
               not needed on older games as it seems (zero for example)
               patch seeking for CRED file so we can avoid a huge hdd file */
            if (irp->seek_offset == 0x1F70000) {
                log_debug("fseek: fake for hdd (0x1F70000)");
                irp->seek_offset = 1024;
                irp->seek_origin = SEEK_SET;
            } else if (irp->seek_offset == 0x1F70800) {
                log_debug("fseek: fake for hdd (0x1F70800)");
                irp->seek_offset = 1024 + 2048;
                irp->seek_origin = SEEK_SET;
            }

            switch (irp->seek_origin) {
                case SEEK_SET:
                    patch_hdd_check_boot_area_fp = (size_t) irp->seek_offset;
                    break;

                case SEEK_CUR:
                    patch_hdd_check_boot_area_fp += irp->seek_offset;
                    break;

                case SEEK_END:
                    patch_hdd_check_boot_area_fp = patch_hdd_check_boot_area_size;
                    patch_hdd_check_boot_area_fp += irp->seek_offset;
                    break;

                default:
                    log_die("Unhandled origin %d", irp->seek_origin);
                    break;
            }

            log_debug("fseek, offset 0x%X, origin %d", irp->seek_offset, irp->seek_origin);

            break;
        }

        case CNH_FILEHOOK_IRP_OP_CLOSE:
            patch_hdd_check_file = NULL;

            log_debug("/dev/hdd fclosed");

            /* Actually close dummy handle */
            return cnh_filehook_invoke_next(irp);

        default:
            log_warn("Unhandled file op: %d", irp->op);
            break;
    }

    return result;
}

static enum cnh_result patch_hdd_check_iohook(struct cnh_iohook_irp *irp)
{
    enum cnh_result result;

    result = CNH_RESULT_SUCCESS;

    assert(irp);

    if ((irp->op != CNH_IOHOOK_IRP_OP_OPEN && irp->fd != patch_hdd_check_fd) &&
            (irp->op != CNH_IOHOOK_IRP_OP_FDOPEN || irp->fdopen_fd != patch_hdd_check_fd)) {
        return cnh_iohook_invoke_next(irp);
    }

    switch (irp->op) {
        case CNH_IOHOOK_IRP_OP_OPEN:
        {
            if (!strcmp(irp->open_filename, "/dev/hdd")) {
                patch_hdd_check_fd = cnh_iohook_open_dummy_fd();
                irp->fd = patch_hdd_check_fd;

                log_debug("/dev/hdd opened: %d", patch_hdd_check_fd);
            } else {
                return cnh_iohook_invoke_next(irp);
            }

            break;
        }

        case CNH_IOHOOK_IRP_OP_FDOPEN:
        {
            patch_hdd_check_file = cnh_filehook_open_dummy_file_handle();
            irp->fdopen_res = patch_hdd_check_file;

            log_debug("/dev/hdd fdopen: %p", patch_hdd_check_file);

            break;
        }

        case CNH_IOHOOK_IRP_OP_READ:
        {
            uint32_t len;

            log_debug("read, pos 0x%X, bytes %d", patch_hdd_check_boot_area_fp, irp->read.nbytes);

            /* read up to eof, only */
            if (patch_hdd_check_boot_area_fp + irp->read.nbytes < patch_hdd_check_boot_area_size) {
                len = irp->read.nbytes;
            } else {
                len = patch_hdd_check_boot_area_size - patch_hdd_check_boot_area_fp;
            }

            memcpy(irp->read.bytes, patch_hdd_check_boot_area + patch_hdd_check_boot_area_fp, len);

            patch_hdd_check_boot_area_fp += len;
            irp->read.pos = len;

            break;
        }

        case CNH_IOHOOK_IRP_OP_SEEK:
        {
            log_debug("Seek: offset %d, orig %d", irp->seek_offset, irp->seek_origin);

            switch (irp->seek_origin) {
                case SEEK_SET:
                    patch_hdd_check_boot_area_fp = (size_t) irp->seek_offset;
                    break;

                case SEEK_CUR:
                    patch_hdd_check_boot_area_fp += irp->seek_offset;
                    break;

                case SEEK_END:
                    patch_hdd_check_boot_area_fp = patch_hdd_check_boot_area_size;
                    patch_hdd_check_boot_area_fp += irp->seek_offset;
                    break;

                default:
                    log_die("Unhandled origin %d", irp->seek_origin);
                    break;
            }

            irp->seek_pos = patch_hdd_check_boot_area_fp;

            break;
        }

        case CNH_IOHOOK_IRP_OP_IOCTL:
        {
            /* hdd information request */
            if (irp->ioctl_req == HDIO_GET_IDENTITY) {
                log_info("ioctl: fake ioctl for hdd (0x30D)");
                /* just return empty data instead of real data */
                struct hd_driveid* drid = (struct hd_driveid*) irp->ioctl.bytes;
                memset(drid, 0, sizeof(struct hd_driveid));
                irp->ioctl.pos = 0;
            }

            break;
        }

        case CNH_IOHOOK_IRP_OP_CLOSE:
            patch_hdd_check_fd = -1;

            log_debug("/dev/hdd closed");

            /* Actually close dummy handle */
            return cnh_iohook_invoke_next(irp);

        default:
            log_warn("Unhandled io op: %d", irp->op);
            break;
    }

    return result;
}

void patch_hdd_check_init(const uint8_t* boot_area_buffer, size_t len)
{
    patch_hdd_check_boot_area = boot_area_buffer;
    patch_hdd_check_boot_area_size = len;

    cnh_iohook_push_handler(patch_hdd_check_iohook);
    cnh_filehook_push_handler(patch_hdd_check_filehook);

    log_info("Initialized: boot area size %d", patch_hdd_check_boot_area_size);
}