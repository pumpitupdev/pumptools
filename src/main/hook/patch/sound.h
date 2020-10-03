/**
 * Patch the sound device to open for sound output. Allows us to use a
 * different device than hw:0 (e.g. HDMI sound output)
 */
#ifndef PATCH_SOUND_H
#define PATCH_SOUND_H

/**
 * Initialize the patch module
 *
 * @param dev_name Sound device name to open instead of whatever pump decides
 */
void patch_sound_init(const char* dev_name);

#endif