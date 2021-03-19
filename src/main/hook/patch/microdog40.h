/**
 * Patch module to install the dongle emulator for a microdog 4.0
 */
#ifndef PATCH_MICRODOG34_H
#define PATCH_MICRODOG34_H

/**
 * Initialize the patch module
 *
 * @param key_data Pointer to loaded key data (dog attributes, key table)
 * @param len Length of the buffer
 */
void patch_microdog40_init(const uint8_t *key_data, size_t len);

/**
 * Shutdown and remove the emulator
 */
void patch_microdog40_shutdown(void);

#endif