/**
 * Install a proper signal handler to enhance error output on crashes
 */
#ifndef PATCH_SIGSEGV_H
#define PATCH_SIGSEGV_H

#include <stdbool.h>

/**
 * Initialize the patch module
 *
 * @param halt_on_segv Halt on segv to attach a debugger to the application
 */
void patch_sigsegv_init(bool halt_on_segv);

#endif