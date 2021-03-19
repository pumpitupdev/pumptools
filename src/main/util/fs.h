#ifndef UTIL_FS_H
#define UTIL_FS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/**
 * Load a file fully into memory
 *
 * @param filename Path of the file to load
 * @param bytes Pointer to a pointer to return an allocated location with the
 *        loaded data to
 * @param nbytes Pointer to a variable to return the read size of the file to
 * @param text_mode True to read the file in text mode (add terminating \0),
 *        false for binary mode
 * @return True on success, false on error
 */
bool util_file_load(
    const char *filename, void **bytes, size_t *nbytes, bool text_mode);

/**
 * Save a buffer to a file (overwrite any existing files with the same name)
 *
 * @param filename Path to save the data to
 * @param bytes Pointer to buffer with contents to save
 * @param nbytes Number of bytes to write to the file
 * @return True on success, false on error
 */
bool util_file_save(const char *filename, const void *bytes, size_t nbytes);

/**
 * Check if a normal file, folder, symlink, ... exists
 *
 * @return True if exists, false otherwise
 */
bool util_fs_path_exists(const char *path);

/**
 * Create a new directory
 *
 * @param path Path to directory
 * @return True if successful, false on failure
 */
bool util_fs_mkdir(const char *path);

/**
 * Create a new empty file
 *
 * @param path Path to the file to create
 * @return True if successful, false on failure
 */
bool util_fs_mkfile(const char *path);

/**
 * Get the path pointing to the file (without the filename)
 *
 * @param path Full path to file
 * @return Newly allocated string (caller has to free) containing the path, only
 */
char *util_fs_get_path_to_file(const char *path);

/**
 * Get the start position of the filename in the path
 *
 * @param path Full path to file
 * @return Newly allocated string (caller has to free) containing the filename,
 *         only
 */
char *util_fs_get_filename(const char *path);

/**
 * Get the absolute path of the give (relative) path
 *
 * @param path (Relative) path
 * @return Allocated string (caller has to free) containing the full absolute
 *         path for the given (relative) path
 */
char *util_fs_get_abs_path(const char *path);

#endif
