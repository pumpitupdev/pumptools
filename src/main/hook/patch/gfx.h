#pragma once

/**
 * Different modes for scaling different source resolutions to target
 * resolutions including keeping the aspect ratio
 */
enum patch_gfx_scale_mode {
    PATCH_GFX_SCALE_MODE_INVALID = 0,
    PATCH_GFX_SCALE_MODE_PIXEL_PERFECT = 1,
    PATCH_GFX_SCALE_MODE_STRETCH = 2
};

/**
 * Initialize the patch module
 *
 * This takes care of running the games on non NVIDIA cards
 */
void patch_gfx_init();

/**
 * Scale the rendered output according to the given mode (see enum)
 *
 * @param scale_mode The scaling mode to apply
 */
void patch_gfx_scale(enum patch_gfx_scale_mode scale_mode);

void patch_gfx_display(uint8_t scaling_mode, uint8_t resizable_window, uint16_t screen_width, uint16_t screen_height, uint16_t frame_limit);