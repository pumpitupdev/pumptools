/**
 * Hook implementation to fix buggy applications that call usb_init repeatetly
 */
#ifndef CNH_USB_INIT_FIX_H
#define CNH_USB_INIT_FIX_H

#include "capnhook/hook/usbhook.h"

/**
 * Hook function to add to the hook module to fix the usb_init bug
 *
 * @param irp I/O request package of hook module received
 * @return Result of operation
 */
enum cnh_result cnh_usb_init_fix(struct cnh_usbhook_irp *irp);

#endif