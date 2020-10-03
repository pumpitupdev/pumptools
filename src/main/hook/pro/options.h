#ifndef PROHOOK_OPTIONS_H
#define PROHOOK_OPTIONS_H

#include <stdbool.h>
#include <stdint.h>

#include "util/log.h"

struct prohook_options {
    struct game {
        const char* data;
        const char* settings;
    } game;

    struct patch {
        struct hook_mon {
            bool file;
            bool fs;
            bool io;
            bool open;
            bool usb;
        } hook_mon;

        struct piubtn {
            const char* api_lib;
        } piubtn;

        struct piuio {
            const char* api_lib;
            bool exit_test_serv;
        } piuio;

        struct usb_profile {
            const char* device_nodes;
            const char* p1_bus_port;
            const char* p2_bus_port;
        } usb_profile;

        struct x11_event_loop {
            const char* api_lib;
            const char* api_lib2;
        } x11_event_loop;
    } patch;

    struct log {
        const char* file;
        enum util_log_level level;
    } log;
};

bool prohook_options_init(int argc, char** argv, struct prohook_options* options);

#endif
