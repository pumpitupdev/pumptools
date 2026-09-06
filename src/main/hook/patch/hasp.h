/**
 * Patch module to install the dongle emulator for a hasp dongle
 */
#ifndef PATCH_HASP_H
#define PATCH_HASP_H

#include <stddef.h>
#include <stdint.h>

/**
 * Initialize the patch module
 *
 * @param key_data Pointer to loaded key data (key table)
 * @param len Length of the buffer
 * @param addr_get_sessioninfo Address of hasp_get_sessioninfo function (0 if not used)
 */
void patch_hasp_init(
    const uint8_t *key_data,
    size_t len,
    uintptr_t addr_get_sessioninfo);

#endif