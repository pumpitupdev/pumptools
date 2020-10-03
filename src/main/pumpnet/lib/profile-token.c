#define LOG_MODULE "pumpnet-lib-profile-token"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "pumpnet/lib/profile-token.h"

#include "util/log.h"

bool pumpnet_lib_profile_token_load(const char* path, uint64_t* profile_id)
{
    FILE* file = fopen(path, "rb");
    ssize_t size;

    if (!file) {
        *profile_id = PUMPNET_LIB_PROFILE_TOKEN_INVALID;
        return false;
    }

    // TODO file needs to be encrypted

    fseek(file, 0, SEEK_END);
    size = ftell(file);

    if (size != 8) {
        fclose(file);
        *profile_id = PUMPNET_LIB_PROFILE_TOKEN_INVALID;
        log_error("Invalid file size (%d) for pumpnet profile %s", size, path);
        return false;
    }

    fseek(file, 0, SEEK_SET);

    if (fread(profile_id, 8, 1, file) != 1) {
        fclose(file);
        *profile_id = PUMPNET_LIB_PROFILE_TOKEN_INVALID;
        log_error("Reading pumpnet profile %s failed: %s", path, strerror(errno));
        return false;
    }

    fclose(file);
    return true;
}