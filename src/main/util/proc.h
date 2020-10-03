#ifndef UTIL_PROC_H
#define UTIL_PROC_H

#include <stdbool.h>
#include <stdlib.h>

/**
 * Check if the current process is running as a root user.
 *
 * @return True if running as root, false otherwise.
 */
bool util_proc_is_running_as_root();

/**
 * Get the full path of the executable of the current process (including the executable name).
 *
 * @param buffer Buffer to read the path into.
 * @param size Size of the buffer
 * @return True on success, false on failure, e.g. buffer too small.
 */
bool util_proc_get_path_executable(char* buffer, size_t size);

/**
 * Get the full path of the folder the executable of the current process is located in.
 *
 * @param buffer Buffer to read the path into.
 * @param size Size of the buffer
 * @return True on success, false on failure, e.g. buffer too small.
 */
bool util_proc_get_folder_path_executable(char* buffer, size_t size);

/**
 * Special version of util_proc_get_folder_path_executable that considers when the process is run explicitly with a
 * ld-linux binary. Returns the path of the application's executable and not ld-linux in that case.
 *
 * @param buffer Buffer to read the path into.
 * @param size Size of the buffer
 * @return True on success, false on failure, e.g. buffer too small.
 */
bool util_proc_get_folder_path_executable_no_ld_linux(char* buffer, size_t size);

/**
 * Log information about the current process to the console.
 */
void util_proc_log_info();

#endif
