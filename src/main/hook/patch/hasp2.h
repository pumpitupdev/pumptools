/**
 * Patch module to install the dongle emulator for a hasp dongle (runs a local server)
 */
#ifndef PATCH_HASP2_H
#define PATCH_HASP2_H

#include <stdint.h>

/**
 * Initialize the patch module and run the hasp server
 *
 * @param key_data Pointer to loaded key data (attributes and key table)
 * @param len Length of the buffer
 */
void patch_hasp_init(const uint8_t* key_data, size_t len);

/**
 * Shutdown and remove the emulator
 */
void patch_hasp_shutdown(void);

#endif
