/**
 * Hook implementation for emulating usb devices on libusb 0.1
 */
#ifndef CNH_USB_EMU_H
#define CNH_USB_EMU_H

#include <stdbool.h>
#include <stdint.h>

#include "capnhook/hook/iobuf.h"
#include "capnhook/hook/usbhook.h"

/**
 * Virtual device endpoint
 */
struct cnh_usb_emu_virtdev_ep {
  uint16_t pid;
  uint16_t vid;
  bool (*enumerate)(bool real_exists);
  enum cnh_result (*open)(void);
  enum cnh_result (*reset)(void);
  enum cnh_result (*control_msg)(
      int request_type,
      int request,
      int value,
      int index,
      struct cnh_iobuf *buffer,
      int timeout);
  void (*close)(void);
};

/**
 * Add a pointer to a virtual device endpoint implementing handler functions
 *
 * @param virtdevep Virtual device endpoint to add
 */
void cnh_usb_emu_add_virtdevep(const struct cnh_usb_emu_virtdev_ep *virtdevep);

/**
 * Hook function to add to the hook module for emulating usb devices
 *
 * @param irp I/O request package of hook module received
 * @return Result of operation
 */
enum cnh_result cnh_usb_emu(struct cnh_usbhook_irp *irp);

#endif