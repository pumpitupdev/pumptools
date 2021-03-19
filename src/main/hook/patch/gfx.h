#pragma once

/**
 * Different modes for scaling different source resolutions to target
 * resolutions including keeping the aspect ratio
 */
enum patch_gfx_scale_mode {
    PATCH_GFX_SCALE_MODE_INVALID = 0,
    PATCH_GFX_SCALE_MODE_SD_480_TO_PILLARBOX_HD_720 = 1,
    PATCH_GFX_SCALE_MODE_SD_480_TO_PILLARBOX_HD_1080 = 2,
    PATCH_GFX_SCALE_MODE_SD_480_TO_SD_960 = 3,
    PATCH_GFX_SCALE_MODE_HD_720_TO_HD_1080 = 4,
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
