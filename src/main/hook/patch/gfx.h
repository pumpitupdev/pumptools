/**
 * Hook module to run the games on non NVIDIA cards
 */
#ifndef PATCH_GFX_H
#define PATCH_GFX_H

/**
 * Initialize the patch module
 */
void patch_gfx_init();

/**
 * Force the game into window mode
 */
void patch_gfx_force_window_mode();

#endif