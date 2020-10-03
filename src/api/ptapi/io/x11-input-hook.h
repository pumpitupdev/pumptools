/**
 * Pumptools hooks into the X11 event dispatch loop to grab different events and exposes
 * keyboard input events. This allows external modules to react to these inputs.
 */
#ifndef PTAPI_IO_X11_INPUT_HOOK_H
#define PTAPI_IO_X11_INPUT_HOOK_H

#include <X11/Xutil.h>

/**
 * Input hooking structure with dispatch functions.
 * Set functions you do not use to NULL.
 */
struct ptapi_io_x11_input_hook_handler {
    /**
     * Dispatch the key press event
     *
     * @param key Key pressed
     */
    void (*dispatch_key_press)(KeySym key);

    /**
     * Dispatch the key release event
     *
     * @param key Key released
     */
    void (*dispatch_key_release)(KeySym key);
};

typedef const struct ptapi_io_x11_input_hook_handler* (*ptapi_io_x11_input_handler_hook_t)(void);

/**
 * Get an input hook handler to receive keyboard inputs from the X11 event loop.
 *
 * @return Either a valid handler structure with dispatch functions or NULL if unused.
 */
const struct ptapi_io_x11_input_hook_handler* ptapi_io_x11_input_handler_hook(void);

#endif
