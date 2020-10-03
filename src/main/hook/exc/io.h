/**
 * Patch module hooking the MK5IO engine code of exceed to inject our own
 * IO code to run the game on newer versions of the piuio as well as custom
 * IO devices
 */
#ifndef EXCHOOK_IO_H
#define EXCHOOK_IO_H

#include "hook/exc/mempatch.h"

/**
 * Initialize MK5IO emulation for exceed
 *
 * @param patch_table Patch table with memory addresses
 * @param piuio_lib_path Path to piuio lib to use for emulation
 * @parma exit_on_service_test Exit the game when pressing service + test
 */
void exchook_io_init(const struct exchook_mempatch_table* patch_table, const char* piuio_lib_path,
    bool exit_on_service_test);

/**
 * Shut down io emulation
 */
void exchook_io_shutdown(void);

#endif