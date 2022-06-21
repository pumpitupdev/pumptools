#define LOG_MODULE "lxio-drv-device"

#include "io/util/usb_.h"

#include "util/log.h"

#include "defs.h"
#include "device.h"

static void *lxio_drv_device_handle;

bool lxio_drv_device_open(void)
{
  if (lxio_drv_device_handle) {
    log_warn("LXIO Device already opened");
    return true;
  }

  lxio_drv_device_handle = io_usb_open(LXIO_VID, LXIO_PID, 1, 0);

  return lxio_drv_device_handle;
}

bool lxio_drv_device_read(uint8_t *buffer, uint8_t len)
{
  int32_t res;

  if (!lxio_drv_device_handle) {
    log_error("Device not opened");
    return false;
  }

  if (len < LXIO_MSG_SIZE) {
    log_error("Read failed, buffer (%d) too small", len);
    return false;
  }

  res = io_usb_interrupt_transfer(
      lxio_drv_device_handle,
      LXIO_ENDPOINT_INPUT,
      buffer,
      LXIO_MSG_SIZE,
      LXIO_DRV_USB_REQ_TIMEOUT);

  if (res != LXIO_MSG_SIZE) {
    log_error("Read failed: %d", res);
    return false;
  }

  return true;
}

bool lxio_drv_device_write(uint8_t *buffer, uint8_t len)
{
  int32_t res;

  if (!lxio_drv_device_handle) {
    log_error("Device not opened");
    return false;
  }

  if (len < LXIO_MSG_SIZE) {
    log_error("Write failed, buffer (%d) too small", len);
    return false;
  }

  res = io_usb_interrupt_transfer(
      lxio_drv_device_handle,
      LXIO_ENDPOINT_OUT,
      buffer,
      LXIO_MSG_SIZE,
      LXIO_DRV_USB_REQ_TIMEOUT);

  if (res != LXIO_MSG_SIZE) {
    log_error("Write failed: %d", res);
    return false;
  }

  return true;
}

void lxio_drv_device_close(void)
{
  if (!lxio_drv_device_handle) {
    log_error("Device not opened");
  }

  io_usb_close(lxio_drv_device_handle);
}