/**
 * Ensures that the mount points /mnt/0 and /mnt/1 exist to allow mounting
 * of connected usb sticks with profile data
 */
#ifndef PATCH_USB_MNT_H
#define PATCH_USB_MNT_H

/**
 * Initialize the patch module
 */
void patch_usb_mnt_init(void);

#endif