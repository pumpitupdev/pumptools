/**
 * Fixes an issue with the engine of 1st and 2nd when redirecting all game data
 * to one game folder, e.g. ./game/.
 */
#ifndef MK3HOOK_1ST_2ND_FS_FIX_H
#define MK3HOOK_1ST_2ND_FS_FIX_H

/**
 * Initialize the module.
 *
 * @param path_game_dir_abs The absolute path to the "game" dir containing all
 * game assets.
 */
void mk3hook_1st_2nd_fs_fix_init(const char *path_game_dir_abs);

#endif
