/**
 * Patch module hooking lockchip calls to a lockchip emulator to decrypt
 * game assets and run the game
 */
#ifndef EXCHOOK_LOCKCHIP_H
#define EXCHOOK_LOCKCHIP_H

#include <stdint.h>

#include "hook/exc/mempatch.h"

/**
 * Initialize lockchip emulation for exceed
 *
 * @param patch_table Patch table with memory addresses
 */
void exchook_lockchip_init(const struct exchook_mempatch_table* patch_table);

#endif