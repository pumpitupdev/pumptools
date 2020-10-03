/**
 * This module fixes different things on the main loop of the game.
 *
 * First, it hooks into the X11 event loop to grab and dispatch inputs for
 * further IO hooks or input emulation.
 *
 * The second thing is quite stupid and fixes crashing on newer kernels
 * because AM used sigalarm to drive certain parts of their engine.
 * This only applies to one generation of the game engine which includes the
 * releases: Exceed, Exceed2, Zero and NX
 *
 * Detailed analysis:
 *
 * AM decided to poll the CPIU::Update function which contains updating
 * the render state as well as decoding sound data, using the signal SIGALRM.
 * This signal is triggered periodically and drives the whole engine.
 * But, starting kernel versions newer than 2.4.27 something changed which
 * causes the game to deadlock an _ALL_ kernels newer than said version. Trying
 * to fix the deadlock issue makes things even worse and only introduces
 * segfaults with random crashes. So this is not the right approach.
 * Starting NX2 AM used a newer kernel and apparently got the same problem
 * with their OS. They fixed it by moving music_send_data from CPIU::Update()
 * to the IO thread. Splitting CPIU::Update that way, this seems to work fine.
 * However, trying this with the old games lacking the patch, we are getting
 * segfaults (again).
 * It looks like there is an issue with firing and/or handling of SIGALRM in
 * another thread than the main thread (don't ask me why). This leads to the
 * solution of the problem: moving the call of CPIU::Update()to the main
 * thread which is only pumping XEvents (i.e. nothing).
 * So let's give the main thread some work: Detouring XPending and calling
 * the registered signal handler for SIGALRM of the game in there and muting
 * signal handling for SIGALRM. This solves all deadlocking and segfault
 * issues which are related to audio playback in combination with decoding a
 * video.
 */
#ifndef PATCH_MAIN_LOOP_H
#define PATCH_MAIN_LOOP_H

#include <stdbool.h>
#include <X11/Xutil.h>

/**
 * Initialize the patch module
 *
 * @param fix_sigalarm_main_loop True to enable the sigalarm thread fix (doc see above)
 * @param disable_built_in_inputs Disable the built in keyboard inputs for test (F1), service (F2) and clear (F3)
 * @param disable_mk3_linux_port_inputs Disable the built-in keyboard inputs on the MK3 linux ports
 */
void patch_main_loop_init(bool fix_sigalarm_main_loop, bool disable_built_in_inputs,
    bool disable_mk3_linux_port_inputs);

/**
 * Register a handler (multiple possible) to receive X11 input events, e.g. key pressed, released
 *
 * @param lib_with_handler_impl Path to a library implementing the x11-input-handler API
 */
void patch_main_loop_add_x11_input_handler(const char* lib_with_handler_impl);

#endif
