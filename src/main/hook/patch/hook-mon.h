/**
 * Patch module for monitoring various system calls. For development and debugging purpose.
 */
#ifndef PATCH_HOOK_MON_H
#define PATCH_HOOK_MON_H

#include <stdbool.h>

/**
 * Initialize the module
 *
 * @param monitor_io True to enable monitoring of I/O calls
 * @param monitor_file True to enable monitoring of file related calls
 * @param monitor_fs True to enable monitoring of file system related calls
 * @param monitor_usb True to enable monitoring of usb calls
 * @param monitor_open True to enable monitoring of any file/device open calls
 */
void patch_hook_mon_init(bool monitor_io, bool monitor_file, bool monitor_fs, bool monitor_usb, bool monitor_open);

#endif
