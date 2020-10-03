//
// Created by on 9/16/18.
//

#ifndef PUMPNET_LIB_PUMPNET_H
#define PUMPNET_LIB_PUMPNET_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "asset/game-version.h"

enum pumpnet_lib_file_type {
    PUMPNET_LIB_FILE_TYPE_SAVE = 0,
    PUMPNET_LIB_FILE_TYPE_RANK = 1,
    PUMPNET_LIB_FILE_TYPE_COUNT = 2,
};

void pumpnet_lib_init(
        enum asset_game_version game,
        const char* server_addr,
        uint64_t machine_id,
        const char* cert_dir_path,
        bool verbose_debug_log);

void pumpnet_lib_shutdown();

bool pumpnet_lib_get(enum pumpnet_lib_file_type file_type, uint64_t player_ref_id, void* buffer, size_t size);

bool pumpnet_lib_put(enum pumpnet_lib_file_type file_type, uint64_t player_ref_id, const void* buffer, size_t size);

#endif
