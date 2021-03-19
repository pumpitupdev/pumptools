#ifndef UTIL_LOG_H
#define UTIL_LOG_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "util/defs.h"

#ifndef LOG_MODULE
#define LOG_MODULE FILENAME
#endif

#ifndef LOG_SUPPRESS

/* Don't expose source file names and absolute paths on public releases */
#ifdef PT_PUBLIC_RELEASE
#undef FILENAME
#define FILENAME ""
#define SOURCE_LINE 0
#else
#define SOURCE_LINE __LINE__
#endif

#define log_debug(...) \
  util_log(LOG_LEVEL_DEBUG, LOG_MODULE, FILENAME, SOURCE_LINE, __VA_ARGS__)
#define log_info(...) \
  util_log(LOG_LEVEL_INFO, LOG_MODULE, FILENAME, SOURCE_LINE, __VA_ARGS__)
#define log_warn(...) \
  util_log(LOG_LEVEL_WARN, LOG_MODULE, FILENAME, SOURCE_LINE, __VA_ARGS__)
#define log_error(...) \
  util_log(LOG_LEVEL_ERROR, LOG_MODULE, FILENAME, SOURCE_LINE, __VA_ARGS__)
#define log_die(...) \
  util_log(LOG_LEVEL_DIE, LOG_MODULE, FILENAME, SOURCE_LINE, __VA_ARGS__)
#define log_die_illegal_state() \
  util_log(LOG_LEVEL_DIE, LOG_MODULE, FILENAME, SOURCE_LINE, "Illegal state")

#else

#define log_debug(...)
#define log_info(...)
#define log_warn(...)
#define log_error(...)
#define log_die(...)

#endif

#define log_assert(x)                            \
  do {                                           \
    if (!(x)) {                                  \
      log_die(FILENAME, __LINE__, __FUNCTION__); \
    }                                            \
  } while (0)

/**
 * Different log levels for logging
 */
enum util_log_level {
  LOG_LEVEL_DIE = 0,
  LOG_LEVEL_ERROR = 1,
  LOG_LEVEL_WARN = 2,
  LOG_LEVEL_INFO = 3,
  LOG_LEVEL_DEBUG = 4
};

/**
 * Open a log file to write log output to in addition to stdout
 *
 * @param path Path of the log file (existing files are overwritten of append
 * argument false)
 * @param append True to append to an existing file, false to create a new file
 * with the specified name
 */
void util_log_set_file(const char *path, bool append);

/**
 * Set the log level for logging
 *
 * @param new_level New log level
 */
void util_log_set_level(enum util_log_level new_level);

/**
 * Get the log file handle (required for hooking to filter it on fwrite calls on
 * the filehook monitor)
 *
 * @return FILE handle of log file or NULL if logging to file disabled
 */
FILE *util_log_get_file_handle(void);

/**
 * Log a message
 *
 * Don't use this call to log something, use the macros instead
 *
 * @param level Log level of the message
 * @param module Name of the module where this function is called
 * @param trace_file Additional trace information, file name
 * @param trace_line Additional trace information, line number
 * @param fmt Format string of log message
 * @param ... Arguments for format string
 */
void util_log(
    enum util_log_level level,
    const char *module,
    const char *trace_file,
    size_t trace_line,
    const char *fmt,
    ...);

#endif