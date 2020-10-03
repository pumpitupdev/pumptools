#define LOG_MODULE "nx2hook-profile-gen"

#include <unistd.h>

#include "capnhook/hook/filehook.h"

#include "asset/nx2/lib/util.h"

#include "util/fs.h"
#include "util/log.h"
#include "util/str.h"

static bool nx2hook_profile_gen_is_creating_file;

// Do not use util_fs_path_exists here as this fopens and causes an endless recursion
static bool nx2hook_profile_gen_check_if_exists(const char* path)
{
    return access(path, F_OK) != -1;
}

static enum cnh_result nx2hook_profile_gen_filehook(struct cnh_filehook_irp *irp)
{
    // nx2hook_profile_gen_is_creating_file: Avoid recursion of any file calls when creating the save/rank files
    if (irp->op != CNH_FILEHOOK_IRP_OP_OPEN || nx2hook_profile_gen_is_creating_file) {
        return cnh_filehook_invoke_next(irp);
    }

    if (util_str_ends_with(irp->open_filename, "nx2save.bin") &&
            !nx2hook_profile_gen_check_if_exists(irp->open_filename)) {
        nx2hook_profile_gen_is_creating_file = true;

        struct asset_nx2_usb_save* save = asset_nx2_usb_save_new();
        asset_nx2_usb_save_finalize(save);

        log_info("No %s file found, generate new", irp->open_filename);

        if (!asset_nx2_usb_save_save_to_file(irp->open_filename, save, true)) {
            log_error("Saving generated nx2save.bin file to %s failed", irp->open_filename);
        }

        free(save);

        nx2hook_profile_gen_is_creating_file = false;
    }

    if (util_str_ends_with(irp->open_filename, "nx2rank.bin") &&
            !nx2hook_profile_gen_check_if_exists(irp->open_filename)) {
        nx2hook_profile_gen_is_creating_file = true;

        struct asset_nx2_usb_rank* rank = asset_nx2_usb_rank_new();
        asset_nx2_usb_rank_finalize(rank);

        log_info("No %s file found, generate new", irp->open_filename);

        if (!asset_nx2_usb_rank_save_to_file(irp->open_filename, rank, true)) {
            log_error("Saving generated nx2rank.bin file to %s failed", irp->open_filename);
        }

        free(rank);

        nx2hook_profile_gen_is_creating_file = false;
    }

    return cnh_filehook_invoke_next(irp);
}

void nx2hook_profile_gen_init(void)
{
    cnh_filehook_push_handler(nx2hook_profile_gen_filehook);
}