#define LOG_MODULE "piuio-drv-device"

#include "io/util/usb_.h"

#include "util/log.h"

#include "defs.h"
#include "device.h"

static void *piuio_drv_device_handle;

bool piuio_drv_device_open(void)
{
  if (piuio_drv_device_handle) {
    log_warn("Device already opened");
    return true;
  }

  piuio_drv_device_handle = io_usb_open(
      PIUIO_DRV_VID, PIUIO_DRV_PID, PIUIO_DRV_CONFIG, PIUIO_DRV_IFACE);

  return piuio_drv_device_handle;
}

bool piuio_drv_device_read(uint8_t *buffer, uint8_t len)
{
  int32_t res;

  if (!piuio_drv_device_handle) {
    log_error("Device not opened");
    return false;
  }

  if (len < PIUIO_DRV_BUFFER_SIZE) {
    log_error("Read failed, buffer (%d) too small", len);
    return false;
  }

  res = io_usb_control_transfer(
      piuio_drv_device_handle,
      PIUIO_DRV_USB_CTRL_TYPE_IN,
      PIUIO_DRV_USB_CTRL_REQUEST,
      0,
      0,
      buffer,
      PIUIO_DRV_BUFFER_SIZE,
      PIUIO_DRV_USB_REQ_TIMEOUT);

  if (res != PIUIO_DRV_BUFFER_SIZE) {
    log_error("Read failed: %d", res);
    return false;
  }

  return true;
}

bool piuio_drv_device_write(uint8_t *buffer, uint8_t len)
{
  int32_t res;

  if (!piuio_drv_device_handle) {
    log_error("Device not opened");
    return false;
  }

  if (len < PIUIO_DRV_BUFFER_SIZE) {
    log_error("Write failed, buffer (%d) too small", len);
    return false;
  }

  res = io_usb_control_transfer(
      piuio_drv_device_handle,
      PIUIO_DRV_USB_CTRL_TYPE_OUT,
      PIUIO_DRV_USB_CTRL_REQUEST,
      0,
      0,
      buffer,
      PIUIO_DRV_BUFFER_SIZE,
      PIUIO_DRV_USB_REQ_TIMEOUT);

  if (res != PIUIO_DRV_BUFFER_SIZE) {
    log_error("Write failed: %d", res);
    return false;
  }

  return true;
}

void piuio_drv_device_close(void)
{
  if (!piuio_drv_device_handle) {
    log_error("Device not opened");
  }

  io_usb_close(piuio_drv_device_handle);
}