/**
 * Starting NX2, the game is checking /proc/mounts for the entry
 * "/dev/hddd /mnt/hd" -> hook and inject that
 */
#ifndef PATCH_MOUNTS_H
#define PATCH_MOUNTS_H

/**
 * Initialize the patch module
 */
void patch_mounts_init(void);

#endif