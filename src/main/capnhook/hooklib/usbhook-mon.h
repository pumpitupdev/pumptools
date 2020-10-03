/**
 * Hook implementation to monitor libusb 0.1 calls
 */
#ifndef CAPNHOOK_USBHOOK_MON_H
#define CAPNHOOK_USBHOOK_MON_H

#include "capnhook/hook/usbhook.h"

/**
 * Hook function to add to the hook module for monitoring libusb calls
 *
 * @param irp I/O request package of hook module received
 * @return Result of operation
 */
enum cnh_result cnh_usbhook_mon(struct cnh_usbhook_irp *irp);

#endif
