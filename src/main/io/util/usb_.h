/**
 * Wrapper module for libusb-1.0 to make it a little more convenient to use
 * usb devices
 */
#ifndef IO_USB_H
#define IO_USB_H

#include <stdint.h>

/**
 * Open a usb device
 *
 * @param vid Vendor ID of the device
 * @param pid Product ID of the device
 * @param config Configuration to set
 * @param iface Interface to claim
 * @return On success, a handle to the opened device, NULL on failure
 */
void* io_usb_open(uint16_t vid, uint16_t pid, uint16_t config, uint16_t iface);

/**
 * Execute a control transfer
 *
 * @param handle Handle of an opened usb device
 * @param request_type Request type
 * @param request Request
 * @param value Value
 * @param index Index
 * @param data Pointer to an allocated buffer (depending on the vendor
 *             specific request type, this can be a read or write buffer)
 * @param len Length of the data to write/read
 * @param timeout Timeout for the operation in ms
 * @return Number of bytes read/written or -1 on error
 */
int32_t io_usb_control_transfer(void* handle, uint8_t request_type,
    uint8_t request, uint16_t value, uint16_t index, uint8_t* data,
    uint16_t len, uint32_t timeout);

/**
 * Close a opened usb device
 *
 * @param handle Handle of the opened device
 */
void io_usb_close(void* handle);

#endif