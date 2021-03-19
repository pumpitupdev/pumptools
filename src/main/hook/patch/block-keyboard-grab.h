#ifndef PATCH_BLOCK_KEYBOARD_GRAB_H
#define PATCH_BLOCK_KEYBOARD_GRAB_H

/**
 * Enable blocking of X11 keyboard grabbing which causes Prime to not go to
 * window mode and behave weird on desktop machines.
 */
void patch_block_keyboard_grab_init();

#endif
