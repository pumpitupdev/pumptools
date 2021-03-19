/**
 * Patch module to install the dongle emulator for a hasp dongle
 */
#ifndef PATCH_HASP_H
#define PATCH_HASP_H

/**
 * Initialize the patch module
 *
 * @param key_data Pointer to loaded key data (key table)
 * @param len Length of the buffer
 */
void patch_hasp_init(const uint8_t *key_data, size_t len);

#endif