#define LOG_MODULE "patch-hook-mon"

#include "capnhook/hooklib/filehook-mon.h"
#include "capnhook/hooklib/fileopen-mon.h"
#include "capnhook/hooklib/fshook-mon.h"
#include "capnhook/hooklib/iohook-mon.h"
#include "capnhook/hooklib/usbhook-mon.h"

#include "util/log.h"

void patch_hook_mon_init(bool monitor_io, bool monitor_file, bool monitor_fs, bool monitor_usb, bool monitor_open)
{
    if (monitor_open) {
        cnh_iohook_push_handler(cnh_fileopen_mon_iohook);
        cnh_filehook_push_handler(cnh_fileopen_mon_filehook);
        cnh_fshook_push_handler(cnh_fileopen_mon_fshook);
    }

    if (monitor_io) {
        cnh_iohook_push_handler(cnh_iohook_mon);
    }

    if (monitor_file) {
        cnh_filehook_push_handler(cnh_filehook_mon);
    }

    if (monitor_fs) {
        cnh_fshook_push_handler(cnh_fshook_mon);
    }

    if (monitor_usb) {
        cnh_usbhook_push_handler(cnh_usbhook_mon);
    }

    log_info("Initialized");
}