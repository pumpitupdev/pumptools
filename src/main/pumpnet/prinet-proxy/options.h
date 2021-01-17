#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "asset/game-version.h"

#include "util/log.h"

struct pumpnet_prinet_proxy_options {
    struct proxy {
        struct source {
            const char* addr;
            uint16_t port;
        } source;

        struct pumpnet {
            const char* server;
            enum asset_game_version game;
            uint64_t machine_id;
            bool verbose_log_output;
            const char* cert_dir_path;
        } pumpnet;
    } proxy;

    struct log {
        const char* file;
        enum util_log_level level;
    } log;
};

bool pumpnet_prinet_proxy_options_init(int argc, char** argv, struct pumpnet_prinet_proxy_options* options);
