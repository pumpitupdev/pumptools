/**
 * Patch module emulating the MK5 (hardware) eeprom chip used to store
 * game settings to on Exceed
 */
#ifndef EXCHOOK_EEPROM_H
#define EXCHOOK_EEPROM_H

#include "hook/exc/mempatch.h"

/**
 * Init eeprom emulation for exceed
 *
 * @param patch_table Patch table with memory addresses
 * @param eeprom_file Path to existing eeprom file to read from and new eeprom
 *                    file to store to
 */
void exchook_eeprom_init(
    const struct exchook_mempatch_table *patch_table, const char *eeprom_file);

#endif