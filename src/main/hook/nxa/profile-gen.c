#define LOG_MODULE "nxahook-profile-gen"

#include <unistd.h>

#include "capnhook/hook/filehook.h"

#include "asset/nxa/lib/util.h"

#include "util/fs.h"
#include "util/log.h"
#include "util/str.h"

static bool nxahook_profile_gen_is_creating_file;

// Do not use util_fs_path_exists here as this fopens and causes an endless recursion
static bool nxahook_profile_gen_check_if_exists(const char* path)
{
    return access(path, F_OK) != -1;
}

static enum cnh_result nxahook_profile_gen_filehook(struct cnh_filehook_irp *irp)
{
    // nx2hook_profile_gen_is_creating_file: Avoid recursion of any file calls when creating the save/rank files
    if (irp->op != CNH_FILEHOOK_IRP_OP_OPEN || nxahook_profile_gen_is_creating_file) {
        return cnh_filehook_invoke_next(irp);
    }

    if (util_str_ends_with(irp->open_filename, "nxasave.bin") &&
            !nxahook_profile_gen_check_if_exists(irp->open_filename)) {
        char* path_only;
        char* path_nx2;
        struct asset_nxa_usb_save* save;

        nxahook_profile_gen_is_creating_file = true;

        /* Check if a nx2save.bin exists and skip generating a new nxasave
           file because the game migrates everything from the nx2 file */
        path_only = util_fs_get_path_to_file(irp->open_filename);
        path_nx2 = util_str_merge(path_only, "/nx2save.bin");
        free(path_only);

        if (util_fs_path_exists(path_nx2)) {
            free(path_nx2);
            log_info("nx2save.bin exists, don't create new nxasave.bin");
            nxahook_profile_gen_is_creating_file = false;
            return cnh_filehook_invoke_next(irp);
        }

        free(path_nx2);

        save = asset_nxa_usb_save_new();
        asset_nxa_usb_save_finalize(save);

        log_info("No %s file found, generate new", irp->open_filename);

        if (!asset_nxa_usb_save_save_to_file(irp->open_filename, save, true)) {
            log_error("Saving generated nxasave.bin file to %s failed", irp->open_filename);
        }

        free(save);

        nxahook_profile_gen_is_creating_file = false;
    }

    if (util_str_ends_with(irp->open_filename, "nxarank.bin") &&
            !nxahook_profile_gen_check_if_exists(irp->open_filename)) {
        char* path_only;
        char* path_nx2;
        struct asset_nxa_usb_rank* rank;

        nxahook_profile_gen_is_creating_file = true;

        /* Check if a nx2rank.bin exists and skip generating a new nxarank
           file because the game migrates everything from the nx2 file */
        path_only = util_fs_get_path_to_file(irp->open_filename);
        path_nx2 = util_str_merge(path_only, "/nx2rank.bin");
        free(path_only);

        if (util_fs_path_exists(path_nx2)) {
            free(path_nx2);
            log_info("nx2rank.bin exists, don't create new nxarank.bin");
            nxahook_profile_gen_is_creating_file = false;
            return cnh_filehook_invoke_next(irp);
        }

        free(path_nx2);

        rank = asset_nxa_usb_rank_new();
        asset_nxa_usb_rank_finalize(rank);

        log_info("No %s file found, generate new", irp->open_filename);

        if (!asset_nxa_usb_rank_save_to_file(irp->open_filename, rank, true)) {
            log_error("Saving generated nxarank.bin file to %s failed", irp->open_filename);
        }

        free(rank);

        nxahook_profile_gen_is_creating_file = false;
    }

    return cnh_filehook_invoke_next(irp);
}

void nxahook_profile_gen_init(void)
{
    cnh_filehook_push_handler(nxahook_profile_gen_filehook);
}