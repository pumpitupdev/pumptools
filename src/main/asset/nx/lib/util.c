#include <string.h>

#include "asset/nx/lib/util.h"

#include "util/fs.h"
#include "util/log.h"

struct asset_nx_settings* asset_nx_util_load_settings_from_file(const char* path)
{
    struct asset_nx_settings* settings;
    size_t size;

    if (!util_file_load(path, (void**) &settings, &size, false)) {
        log_error("Loading nx setting file %s failed", path);
        return NULL;
    }

    if (size != sizeof(struct asset_nx_settings)) {
        log_error("Invalid size of nx setting file %s: %d != %d", path, size, sizeof(struct asset_nx_settings));
        free(settings);
        return NULL;
    }

    return settings;
}

bool asset_nx_util_save_settings_to_file(const char* path, const struct asset_nx_settings* settings)
{
    if (!util_file_save(path, (void**) settings,  sizeof(struct asset_nx_settings))) {
        log_error("Storing nx settings data to %s failed", path);
        return false;
    }

    return true;
}