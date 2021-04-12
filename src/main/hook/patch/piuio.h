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
 * @param poll_delay_ms If the polling thread relies on the hardware
 *  load for timing, CPU load increases massively if the caller is not taking
 *  care of sleeping properly. Injects an artificial delay to emulate that and
 *  control the CPU load.
 */
void patch_piuio_init(const char *piuio_lib_path, uint32_t poll_delay_ms);

/**
 * Shut down the patch module
 */
void patch_piuio_shutdown(void);

#endif