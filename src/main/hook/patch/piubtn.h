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
 * @param real_passthrough True to ignore the library and disable any emulation
 */
void patch_piubtn_init(const char* piubtn_lib_path);

/**
 * Get the input hook handler provided by the piubtn library.
 *
 * @return Input hook handler for main-loop patch module or NULL if none is used.
 */
const struct pumptools_input_hook_handler* patch_piubtn_get_input_hook_handler(void);

/**
 * Shut down the patch module
 */
void patch_piubtn_shutdown(void);

#endif