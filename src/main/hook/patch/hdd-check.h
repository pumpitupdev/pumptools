/**
 * Patch the HDD check which (normally) ensures that the game is running on a
 * real HDD with valid data written to the boot area
 */
#ifndef PATCH_HDD_CHECK_H
#define PATCH_HDD_CHECK_H

/**
 * Initialize the patch module
 *
 * @param boot_area_buffer Buffer with boot area sections of a real drive
 * @param len Length of buffer provided
 */
void patch_hdd_check_init(const uint8_t* boot_area_buffer, size_t len);

#endif