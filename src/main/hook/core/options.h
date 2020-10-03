#ifndef HOOK_CORE_OPTIONS_H
#define HOOK_CORE_OPTIONS_H

#include "util/log.h"

struct hook_core_options {
    struct log {
        const char* file;
        enum util_log_level level;
    } log;
};

void hook_core_options_init(int argc, char** argv, struct hook_core_options* options);

#endif
