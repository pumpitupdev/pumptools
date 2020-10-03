#ifndef UTIL_STR_H
#define UTIL_STR_H

#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * Check if a string starts with a specific (sub-) string
 *
 * @param haystack String to check
 * @param needle String to compare with start of haystack
 * @return True if haystack ends with needle, false otherwise
 */
bool util_str_starts_with(const char* haystack, const char* needle);

/**
 * Check if a string ends with a specific (sub-) string
 *
 * @param haystack String to check
 * @param needle String to compare with end of haystack
 * @return True if haystack ends with needle, false otherwise
 */
bool util_str_ends_with(const char* haystack, const char* needle);

/**
 * Print a formated string to a buffer
 *
 * @param buf Allocated buffer to print to
 * @param nchars Size of allocated buffer
 * @param fmt Format of the string to print
 * @param ... Arguments for the format string
 * @return Size of the string printed to the buffer
 */
size_t util_str_format(char* buf, size_t nchars, const char* fmt, ...);

/**
 * Print a formated string to a buffer
 *
 * @param buf Allocated buffer to print to
 * @param nchars Size of allocated buffer
 * @param fmt Format of the string to print
 * @param ap Argument list for format
 * @return Size of the string printed to the buffer
 */
size_t util_str_vformat(char* buf, size_t nchars, const char* fmt, va_list ap);

/**
 * Concat a string to another one
 *
 * @param dest Destination to append to
 * @param dnchars Size of dest buffer
 * @param src Source string to concat to dest
 */
void util_str_cat(char* dest, size_t dnchars, const char* src);

/**
 * Copy a string
 *
 * @param dest Destination to copy to
 * @param dnchars Size of destination
 * @param src Source string to copy from (to dest)
 */
void util_str_cpy(char* dest, size_t dnchars, const char* src);

/**
 * Merge to strings (str1 + str2)
 *
 * @param str1 String 1 to merge
 * @param str2 String 2 to merge
 * @return Newly allocated merged string: str1 + str2. Caller has to manage
 *         memory returned
 */
char* util_str_merge(const char* str1, const char* str2);

/**
 * Duplicate a string
 *
 * @param src String to duplicate
 * @return Duplicated string (newly allocated memory). Caller has to manage
 *         memory returned
 */
char* util_str_dup(const char* src);

/**
 * Remove trailing spaces of a string
 *
 * @param str String to trim
 */
void util_str_trim(char* str);

/**
 * Print a buffer as hex digits to a string
 *
 * @param buf Buffer to print
 * @param len Length of the buffer
 * @return String representation of the buffer. Caller has to manage returned
 *         memory
 */
char* util_str_buffer(const uint8_t* buf, size_t len);

char** util_str_split(const char* str, const char* delim, size_t* count);

void util_str_free_split(char** split, size_t count);

#endif
