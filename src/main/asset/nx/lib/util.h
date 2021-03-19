#ifndef ASSET_NX_UTIL_H
#define ASSET_NX_UTIL_H

#include <stdbool.h>

#include "asset/nx/lib/settings.h"

struct asset_nx_settings *
asset_nx_util_load_settings_from_file(const char *path);

bool asset_nx_util_save_settings_to_file(
    const char *path, const struct asset_nx_settings *settings);

#endif
