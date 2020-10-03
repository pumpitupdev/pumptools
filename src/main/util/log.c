#define LOG_MODULE "util-log"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "util/defs.h"
#include "util/log.h"
#include "util/str.h"
#include "util/time.h"

#define COLORIZING_CONTROL_SEQ "\033[%d;%dm%s\033[0m"

static void util_log_format(enum util_log_level level, const char* module,
        const char* trace_file, size_t trace_line, const char* fmt, va_list ap);
static void util_log_writer_console(void* ctx, enum util_log_level level, const char* module, const char* trace_file,
        size_t trace_line, const char* msg);
static void util_log_writer_file(void* ctx, enum util_log_level level, const char* module, const char* trace_file,
        size_t trace_line, const char* msg);

static const char util_log_level_chars[] = "DEWIM";

static enum util_log_level util_log_level = LOG_LEVEL_DEBUG;
static FILE* util_log_file;

void util_log_set_file(const char* path, bool append)
{
    const char* mode = append ? "a+" : "w+";

    if (path) {
        util_log_file = fopen(path, mode);
        log_info("Open log file: %s", path);
    }
}

void util_log_set_level(enum util_log_level new_level)
{
    util_log_level = new_level;
}

FILE* util_log_get_file_handle(void)
{
    return util_log_file;
}

void util_log(enum util_log_level level, const char* module, const char* trace_file, size_t trace_line,
        const char* fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    util_log_format(level, module, trace_file, trace_line, fmt, ap);
    va_end(ap);
}

static void util_log_colorize_text(enum util_log_level level, const char* text, char* buffer)
{
    switch (level) {
        case LOG_LEVEL_DEBUG:
            sprintf(buffer, COLORIZING_CONTROL_SEQ, 0, 37, text);
            break;

        case LOG_LEVEL_INFO:
            sprintf(buffer, COLORIZING_CONTROL_SEQ, 0, 34, text);
            break;

        case LOG_LEVEL_WARN:
            sprintf(buffer, COLORIZING_CONTROL_SEQ, 0, 33, text);
            break;

        case LOG_LEVEL_ERROR:
            sprintf(buffer, COLORIZING_CONTROL_SEQ, 0, 31, text);
            break;

        case LOG_LEVEL_DIE:
            sprintf(buffer, COLORIZING_CONTROL_SEQ, 0, 31, text);
            break;

        default:
            log_die_illegal_state();
            break;
    }
}

static void util_log_format(enum util_log_level level, const char* module,
        const char* trace_file, size_t trace_line, const char* fmt, va_list ap)
{
    char msg[8192];

    if (level <= util_log_level) {
        util_str_vformat(msg, sizeof(msg), fmt, ap);

        util_log_writer_console(NULL, level, module, trace_file, trace_line, msg);

        if (util_log_file != NULL) {
            util_log_writer_file(util_log_file, level, module, trace_file, trace_line, msg);
        }
    }

    if (level == LOG_LEVEL_DIE) {
        abort();
    }
}

static void util_log_writer_console(void* ctx, enum util_log_level level, const char* module, const char* trace_file,
        size_t trace_line, const char* msg)
{
    char text_level[128];
    char text_level_color[128];

    util_str_format(text_level, sizeof(text_level), "[%c]", util_log_level_chars[level]);
    util_log_colorize_text(level, text_level, text_level_color);

    // ---------------------------

    struct util_time_timestamp timestamp;

    util_time_get_current_time(&timestamp);

    // ---------------------------

    char line[8192];

    util_str_format(line, sizeof(line), "%s[%d/%d/%d-%d:%d:%d:%d][%s][%s:%d] %s\n", text_level_color, timestamp.year,
            timestamp.month, timestamp.day, timestamp.hour, timestamp.min, timestamp.sec, timestamp.millisec,
            module, trace_file, trace_line, msg);

    printf("%s", line);
}

static void util_log_writer_file(void* ctx, enum util_log_level level, const char* module, const char* trace_file,
        size_t trace_line, const char* msg)
{
    struct util_time_timestamp timestamp;

    util_time_get_current_time(&timestamp);

    // ---------------------------

    char line[8192];
    size_t line_len;

    line_len = util_str_format(line, sizeof(line), "[%c][%d/%d/%d-%d:%d:%d:%d][%s][%s:%d] %s\n",
            util_log_level_chars[level], timestamp.year, timestamp.month, timestamp.day, timestamp.hour, timestamp.min,
            timestamp.sec, timestamp.millisec, module, trace_file, trace_line, msg);

    fwrite(line, 1, line_len, (FILE*) ctx);
    fflush((FILE*) ctx);
}