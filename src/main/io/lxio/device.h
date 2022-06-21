/**
 * Module for sending and recv raw data to the "lxio"/atmegapump on the
 * interrupt endpoint.
 */
#ifndef LXIO_DRV_DEVICE_H
#define LXIO_DRV_DEVICE_H

#include <stdbool.h>
#include <stdint.h>

/**
 * Open a connected lxio device
 *
 * @return True if opening a lxio device was successful, false on failure
 */
bool lxio_drv_device_open(void);

/**
 * Read raw data from the lxio device
 *
 * @param buffer Allocated buffer to read data from the device into
 * @param len Number of bytes to read
 * @return True if reading data to device was successful, false otherwise
 */
bool lxio_drv_device_read(uint8_t *buffer, uint8_t len);

/**
 * Write raw data to the lxio device
 *
 * @param buffer Buffer with data to write
 * @param len Number of bytes to write
 * @return True if writing data to device was successful, false otherwise
 */
bool lxio_drv_device_write(uint8_t *buffer, uint8_t len);

/**
 * Close the opened lxio device
 */
void lxio_drv_device_close(void);

#endif