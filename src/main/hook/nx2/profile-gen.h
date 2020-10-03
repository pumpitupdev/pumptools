/**
 * Patch module for NX2 to enable auto generating of save files on empty/non
 * Andamiro usb drives
 */
#ifndef NX2HOOK_PROFILE_GEN_H
#define NX2HOOK_PROFILE_GEN_H

/**
 * Initialize this patch module to allow auto generation of new/empty profiles
 * on usb sticks that do not contain stock NX2 profile data
 */
void nx2hook_profile_gen_init(void);

#endif