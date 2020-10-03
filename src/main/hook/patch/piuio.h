/**
 * Patch libusb-0.1 calls to intercept with piuio communication and hook
 * our API with emulation code
 */
#ifndef PATCH_PIUIO_H
#define PATCH_PIUIO_H

#include "util/options.h"

/**
 * Initialize the patch module
 *
 * @param piuio_lib_path Path to a library implementing the piuio API
 */
void patch_piuio_init(const char* piuio_lib_path);

/**
 * Shut down the patch module
 */
void patch_piuio_shutdown(void);

#endif