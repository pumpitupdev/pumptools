/**
 * Patch module to force unlock all songs/features of the game without modifying the configuration stored on disk.
 */
#ifndef HOOK_NX_UNLOCK_H
#define HOOK_NX_UNLOCK_H

/**
 * Initialize the patch module force unlocking all locked game content.
 */
void hook_nx_unlock_init(void);

#endif
