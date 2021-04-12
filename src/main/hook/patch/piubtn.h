/**
 * Patch libusb-0.1 calls to intercept with piubtn (pump pro buttons)
 * communication and hook our API with emulation code
 */
#ifndef PATCH_PIUBTN_H
#define PATCH_PIUBTN_H

#include "ptapi/io/x11-input-hook.h"

#include "util/options.h"

/**
 * Initialize the patch module
 *
 * @param piubtn_lib_path Path to a library implementing the piubtn API
 * @param poll_delay_ms If the polling thread relies on the hardware
 *  load for timing, CPU load increases massively if the caller is not taking
 *  care of sleeping properly. Injects an artificial delay to emulate that and
 *  control the CPU load.
 */
void patch_piubtn_init(const char *piubtn_lib_path, uint32_t poll_delay_ms);

/**
 * Get the input hook handler provided by the piubtn library.
 *
 * @return Input hook handler for main-loop patch module or NULL if none is
 * used.
 */
const struct pumptools_input_hook_handler *
patch_piubtn_get_input_hook_handler(void);

/**
 * Shut down the patch module
 */
void patch_piubtn_shutdown(void);

#endif