#ifndef PROHOOK_X11_EVENT_LOOP_H
#define PROHOOK_X11_EVENT_LOOP_H

/**
 * Initialize this module which re-implements a X11 event loop processing
 * keyboard button presses and releases. This is required for Pro 1/2 because
 * the game has all X11 input handling removed. Therefore, we cannot use any
 * ptapi io implementations using X11, e.g. keyboard inputs.
 */
void patch_x11_event_loop_init();

/**
 * Register a handler (multiple possible) to receive X11 input events, e.g. key
 * pressed, released
 *
 * @param lib_with_handler_impl Path to a library implementing the
 * x11-input-handler API
 */
void patch_x11_event_loop_add_input_handler(const char *lib_with_handler_impl);

#endif
