/**
 * The kernels used by nx2 and nxa support usbfs. Newer kernels
 * don't have this module anymore. This module takes care of fixing any
 * issues not having the module.
 */
#ifndef PATCH_USBFS_H
#define PATCH_USBFS_H

/**
 * Initialize the patch module
 */
void patch_usbfs_init();

#endif