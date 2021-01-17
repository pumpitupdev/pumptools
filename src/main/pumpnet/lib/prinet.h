#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "asset/game-version.h"

void pumpnet_lib_prinet_init(
        enum asset_game_version game,
        const char* server_addr,
        uint64_t machine_id,
        const char* cert_dir_path,
        bool verbose_debug_log);

void pumpnet_lib_prinet_shutdown();

ssize_t pumpnet_lib_prinet_msg(const void* req_buffer, size_t req_size, void* resp_buffer, size_t max_resp_size);