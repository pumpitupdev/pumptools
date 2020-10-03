/**
 * Patch module for NXA to enable auto generating of save files on empty/non
 * Andamiro usb drives
 */
#ifndef NXAHOOK_PROFILE_GEN_H
#define NXAHOOK_PROFILE_GEN_H

/**
 * Initialize this patch module to allow auto generation of new/empty profiles
 * on usb sticks that do not contain stock NXA profile data
 */
void nxahook_profile_gen_init(void);

#endif