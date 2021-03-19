/**
 * This patch module fixes a bunch of issues with fmodex which is used by the
 * MK3 Linux ports to re-implement the sound hardware in software.
 */
#ifndef MK3HOOK_FMODEX_H
#define MK3HOOK_FMODEX_H

#include <stdbool.h>

/**
 * Initialize this patch module.
 *
 * @param debug_output True to enable fmodex's debug output which requires the
 * usage of fmodexL.so instead of fmodex.so.
 * @param sound_device Provide a sound device to pick from the list of devices
 * available with fmodex. NULL to go with with default device selected
 * automatically by fmodex.
 */
void mk3hook_fmodex_init(bool debug_output, const char *sound_device);

#endif
