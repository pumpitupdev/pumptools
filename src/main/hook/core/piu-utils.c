#define LOG_MODULE "hook-core-piu-utils"

#include <locale.h>
#include <stdlib.h>
#include <string.h>

#include "hook/core/options.h"

#include "util/fs.h"
#include "util/glibc.h"
#include "util/log.h"
#include "util/proc.h"
#include "util/str.h"
#include "util/sys-info.h"

static void hook_core_piu_log_application_args(int argc, char** argv)
{
    const char* sep = "\n";
    char* str = util_str_dup("");
    char* tmp;

    for (int i = 0; i < argc; i++) {
        if (strlen(str) > 0) {
            tmp = util_str_merge(str, sep);
            free(str);
            str = tmp;
        }

        tmp = util_str_merge(str, argv[i]);
        free(str);
        str = tmp;
    }

    log_info("Application arguments (%d):\n%s", argc, str);
    free(str);
}

void hook_core_piu_log_init(int argc, char** argv)
{
    struct hook_core_options options;

    hook_core_options_init(argc, argv, &options);

    util_log_set_file(options.log.file, false);
    util_log_set_level(options.log.level);
}

void hook_core_piu_log_info(int argc, char** argv)
{
    log_info("\n"
         "                              _              _     \n"
         "  _ __  _   _ _ __ ___  _ __ | |_ ___   ___ | |___ \n"
         " | '_ \\| | | | '_ ` _ \\| '_ \\| __/ _ \\ / _ \\| / __|\n"
         " | |_) | |_| | | | | | | |_) | || (_) | (_) | \\__ \\\n"
         " | .__/ \\__,_|_| |_| |_| .__/ \\__\\___/ \\___/|_|___/\n"
         " |_|                   |_|                         ");

    log_info("Pumptools build " __DATE__ " " __TIME__ " gitrev %s", STRINGIFY(GITREV));

    util_sys_info_log();
    util_proc_log_info();
    util_glibc_info_log();
    hook_core_piu_log_application_args(argc, argv);
}

char* hook_core_piu_utils_get_and_verify_game_data_path(int argc, char** argv)
{
    if (argc < 2) {
        log_error("Missing game data path, usage for piu (stock args): piu <game folder> (e.g. piu ./game)");
        exit(0);
    }

    char* game_data_path = util_str_dup(argv[1]);

    log_info("Game data path: %s", game_data_path);

    /* remove trailing / for the hook. however, the game crashes without it */
    if (util_str_ends_with(game_data_path, "/")) {
        game_data_path[strlen(game_data_path) - 1] = '\0';
    } else {
        log_error("Game data path '%s' does not end with a trailing /, exiting because the game is going to "
            "crash anyway!", game_data_path);
        exit(0);
    }

    /* depending on the version, the game also just crashes if the game folder or files within it were not found */
    if (!util_fs_path_exists(game_data_path)) {
        log_error("Game data path '%s' does not exist, exiting", game_data_path);
        exit(0);
    }

    return game_data_path;
}

void hook_core_piu_utils_verify_root_user()
{
    log_info("Verifying process is running as root user.");

    if (!util_proc_is_running_as_root()) {
        log_error("Process not running as root which is required for the game to work correctly.");
        //exit(0);
    }
}

void hook_core_piu_utils_fix_locale()
{
    log_info("Fixing locale settings.");

    if (setenv("LC_ALL", "C", 1) != 0) {
        log_warn("Setting locale LC_ALL failed.");
    }

    if (unsetenv("LANGUAGE") != 0) {
        log_warn("Unsetting LANGUAGE env var failed.");
    }
}

bool hook_core_piu_block_non_piu_process_recursion(int argc, char** argv)
{
    /* Sanity check for arguments. If further child processes are started
       from the current process, and using LD_PRELOAD on the current process,
       all children are also getting the preloaded library. This causes
       multiple calls to the trap main function and just crashes our
       application at some point.
       The executable file is required to contain the "piu" substring to make
       this work properly */
    if (!strstr(argv[0], "piu")) {
        log_warn("Not attaching to non piu process %s", argv[0]);
        return true;
    } else {
        return false;
    }
}