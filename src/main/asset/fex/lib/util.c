#define LOG_MODULE "asset-fex-util"

#include <string.h>

#include "util/fs.h"
#include "util/log.h"

#include "util.h"

struct asset_fex_usb_rank* asset_fex_usb_rank_load_from_file(const char* path,
        bool encrypted)
{
    struct asset_fex_usb_rank* rank;
    size_t size;

    if (!util_file_load(path, (void**) &rank, &size, false)) {
        log_error("Loading fex rank file %s failed", path);
        return NULL;
    }

    // if (size != sizeof(struct fex_profile_rank)) {
    //     log_error("Invalid size of fex rank file %s: %d != %d", path, size,
    //         sizeof(struct fex_profile_rank));
    //     free(rank);
    //     return NULL;
    // }

    if (encrypted) {
        asset_fex_usb_rank_decrypt((uint8_t*) rank, size);
    }

    return rank;
}

bool asset_fex_usb_rank_save_to_file(const char* path,
        const struct asset_fex_usb_rank* rank, bool encrypt)
{
    struct asset_fex_usb_rank rank_enc;

    memcpy(&rank_enc, rank, sizeof(struct asset_fex_usb_rank));

    if (encrypt) {
        asset_fex_usb_rank_encrypt((uint8_t*) &rank_enc,
                sizeof(struct asset_fex_usb_rank));
    }

    if (!util_file_save(path, (void**) &rank_enc,
            sizeof(struct asset_fex_usb_rank))) {
        log_error("Storing fex rank data to %s failed", path);
        return false;
    }

    return true;
}

struct asset_fex_usb_save* asset_fex_usb_save_load_from_file(const char* path,
        bool encrypted)
{
    struct asset_fex_usb_save* save;
    size_t size;

    if (!util_file_load(path, (void**) &save, &size, false)) {
        log_error("Loading fex save file %s failed", path);
        return NULL;
    }

    // if (size != sizeof(struct fex_profile_save)) {
    //     log_error("Invalid size of fex save file %s: %d != %d", path, size,
    //         sizeof(struct fex_profile_save));
    //     free(save);
    //     return NULL;
    // }

    if (encrypted) {
        asset_fex_usb_save_decrypt((uint8_t*) save, size);
    }

    return save;
}

bool asset_fex_usb_save_save_to_file(const char* path,
        const struct asset_fex_usb_save* save, bool encrypt)
{
    struct asset_fex_usb_save save_enc;

    memcpy(&save_enc, save, sizeof(struct asset_fex_usb_save));

    if (encrypt) {
        asset_fex_usb_save_encrypt((uint8_t*) &save_enc,
                sizeof(struct asset_fex_usb_save));
    }

    if (!util_file_save(path, (const void*) &save_enc,
            sizeof(struct asset_fex_usb_save))) {
        log_error("Storing fex save data to %s failed", path);
        return false;
    }

    return true;
}