//
// Created by on 9/16/18.
//

#ifndef PUMPNET_LIB_PROFILE_TOKEN_H
#define PUMPNET_LIB_PROFILE_TOKEN_H

#include <stdbool.h>
#include <stdint.h>

#define PUMPNET_LIB_PROFILE_TOKEN_INVALID 0xFFFFFFFFFFFFFFFF

bool pumpnet_lib_profile_token_load(const char* path, uint64_t* profile_id);

#endif
