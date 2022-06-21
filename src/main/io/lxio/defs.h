#ifndef LXIO_DRV_DEFS_H
#define LXIO_DRV_DEFS_H

#define LXIO_VID 0x0D2F
#define LXIO_PID 0x1020

#define LXIO_DRV_USB_REQ_TIMEOUT 10000

// NOTE: usb.c takes care of setting the MSB for the endpoint address.
#define LXIO_ENDPOINT_INPUT 0x81
#define LXIO_ENDPOINT_OUT 0x02

#define LXIO_MSG_SIZE 16

#endif