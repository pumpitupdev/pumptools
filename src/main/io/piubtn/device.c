#define LOG_MODULE "piubtn-drv-device"

#include "io/util/usb_.h"

#include "util/log.h"

#include "defs.h"
#include "device.h"

static void *piubtn_drv_device_handle;

bool piubtn_drv_device_open(void)
{
  if (piubtn_drv_device_handle) {
    log_warn("Device already opened");
    return true;
  }

  piubtn_drv_device_handle = io_usb_open(
      PIUBTN_DRV_VID, PIUBTN_DRV_PID, PIUBTN_DRV_CONFIG, PIUBTN_DRV_IFACE);

  return piubtn_drv_device_handle;
}

bool piubtn_drv_device_read(uint8_t *buffer, uint8_t len)
{
  int32_t res;

  if (!piubtn_drv_device_handle) {
    log_error("Device not opened");
    return false;
  }

  if (len < PIUBTN_DRV_BUFFER_SIZE) {
    log_error("Read failed, buffer (%d) too small", len);
    return false;
  }

  res = io_usb_control_transfer(
      piubtn_drv_device_handle,
      PIUBTN_DRV_USB_CTRL_TYPE_IN,
      PIUBTN_DRV_USB_CTRL_REQUEST,
      0,
      0,
      buffer,
      PIUBTN_DRV_BUFFER_SIZE,
      PIUBTN_DRV_USB_REQ_TIMEOUT);

  if (res != PIUBTN_DRV_BUFFER_SIZE) {
    log_error("Read failed: %d", res);
    return false;
  }

  return true;
}

bool piubtn_drv_device_write(uint8_t *buffer, uint8_t len)
{
  int32_t res;

  if (!piubtn_drv_device_handle) {
    log_error("Device not opened");
    return false;
  }

  if (len < PIUBTN_DRV_BUFFER_SIZE) {
    log_error("Write failed, buffer (%d) too small", len);
    return false;
  }

  res = io_usb_control_transfer(
      piubtn_drv_device_handle,
      PIUBTN_DRV_USB_CTRL_TYPE_OUT,
      PIUBTN_DRV_USB_CTRL_REQUEST,
      0,
      0,
      buffer,
      PIUBTN_DRV_BUFFER_SIZE,
      PIUBTN_DRV_USB_REQ_TIMEOUT);

  if (res != PIUBTN_DRV_BUFFER_SIZE) {
    log_error("Write failed: %d", res);
    return false;
  }

  return true;
}

void piubtn_drv_device_close(void)
{
  if (!piubtn_drv_device_handle) {
    log_error("Device not opened");
  }

  io_usb_close(piubtn_drv_device_handle);
}