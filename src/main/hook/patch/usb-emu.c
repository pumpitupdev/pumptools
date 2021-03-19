#define LOG_MODULE "patch-usb-emu"

#include "capnhook/hooklib/usb-emu.h"

#include "util/log.h"

void patch_usb_emu_init()
{
  cnh_usbhook_push_handler(&cnh_usb_emu);

  log_info("Initialized");
}