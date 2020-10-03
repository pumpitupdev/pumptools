#define LOG_MODULE "patch-usb-init-fix"

#include "capnhook/hooklib/usb-init-fix.h"

#include "util/log.h"

void patch_usb_init_fix_init()
{
    cnh_usbhook_push_handler(&cnh_usb_init_fix);

    log_info("Initialized");
}