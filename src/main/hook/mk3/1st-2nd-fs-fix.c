#define LOG_MODULE "mk3hook-1st-2nd-fs-fix"

#include <string.h>

#include "capnhook/hook/lib.h"

#include "util/log.h"
#include "util/str.h"

typedef int (*cnh_fshook_chdir_t)(const char *path);
static cnh_fshook_chdir_t mk3hook_1st_2nd_fs_fix_real_chdir;

static char* mk3hook_1st_2nd_fs_fix_path_game_dir_abs;

int chdir(const char *path)
{
    if (!mk3hook_1st_2nd_fs_fix_real_chdir) {
        mk3hook_1st_2nd_fs_fix_real_chdir = (cnh_fshook_chdir_t) cnh_lib_get_func_addr("chdir");
    }

    // This seems to be some kind of probe the game is doing to determine something...I assume which folder layout to
    // use? When we change directory to our "game" dir which contains all stuff at that point, everything's fine.
    // If we do not do that, the game will error not being able to find any files. When loading files, the code also
    // does some odd iterating the file system starting at / at that point. I assume that this is some sort of search
    // trying to look for the file on the filesystem. However, that breaks a lot of other stuf with the hooking code
    // and therefore should not be triggered.
    if (mk3hook_1st_2nd_fs_fix_path_game_dir_abs && !strcmp(path, "/datas/bga")) {
        log_debug("Trapping /datas/bga chdir, chdir to %s", mk3hook_1st_2nd_fs_fix_path_game_dir_abs);

        return mk3hook_1st_2nd_fs_fix_real_chdir(mk3hook_1st_2nd_fs_fix_path_game_dir_abs);
    } else {
        return mk3hook_1st_2nd_fs_fix_real_chdir(path);
    }
}

void mk3hook_1st_2nd_fs_fix_init(const char* path_game_dir_abs)
{
    mk3hook_1st_2nd_fs_fix_path_game_dir_abs = util_str_dup(path_game_dir_abs);

    log_info("Initialized");
}