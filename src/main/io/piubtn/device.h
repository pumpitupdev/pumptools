/**
 * This module serves as a low level device implementation for a usb piubtn
 * io. It uses libusb-1.0 to communicate with the usb device.
 */
#ifndef PIUBTN_DRV_DEVICE_H
#define PIUBTN_DRV_DEVICE_H

#include <stdbool.h>
#include <stdint.h>

/**
 * Open a connected usb piubtn device
 *
 * @return True if opening a piubtn device was successful, false on failure
 */
bool piubtn_drv_device_open(void);

/**
 * Read raw data from the piubtn device
 *
 * @param buffer Allocated buffer to read data from the device into
 * @param len Number of bytes to read
 * @return True if reading data to device was successful, false otherwise
 */
bool piubtn_drv_device_read(uint8_t *buffer, uint8_t len);

/**
 * Write raw data to the piubtn device
 *
 * @param buffer Buffer with data to write
 * @param len Number of bytes to write
 * @return True if writing data to device was successful, false otherwise
 */
bool piubtn_drv_device_write(uint8_t *buffer, uint8_t len);

/**
 * Close the opened piubtn device
 */
void piubtn_drv_device_close(void);

#endif