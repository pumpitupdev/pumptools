/**
 * Hook library for Pump It Up: NXA
 */
#define LOG_MODULE "nxahook"

#include <string.h>

#include "capnhook/hook/lib-main.h"
#include "capnhook/hooklib/redir.h"

#include "hook/core/piu-utils.h"

#include "hook/patch/gfx.h"
#include "hook/patch/hdd-check.h"
#include "hook/patch/hook-mon.h"
#include "hook/patch/main-loop.h"
#include "hook/patch/microdog40.h"
#include "hook/patch/mounts.h"
#include "hook/patch/net-profile.h"
#include "hook/patch/piuio.h"
#include "hook/patch/piuio-exit.h"
#include "hook/patch/redir.h"
#include "hook/patch/sigsegv.h"
#include "hook/patch/sound.h"
#include "hook/patch/usb-emu.h"
#include "hook/patch/usb-init-fix.h"
#include "hook/patch/usb-mnt.h"
#include "hook/patch/usb-unlock.h"
#include "hook/patch/usbfs.h"

#include "util/fs.h"
#include "util/glibc.h"
#include "util/str.h"
#include "util/sys-info.h"

#include "options.h"
#include "profile-gen.h"

/* Compiled binary data from data folder. Symbols are defined by compiler */
extern const uint8_t _binary_nxahook_hdd_raw_start[];
extern const uint8_t _binary_nxahook_hdd_raw_end[];
extern const uint8_t _binary_nxahook_dog_key_start[];
extern const uint8_t _binary_nxahook_dog_key_end[];

static void nxahook_bootstrapping(int argc, char** argv, struct nxahook_options* options, char** game_data_path)
{
    log_assert(argc >= 0);
    log_assert(argv);
    log_assert(options);
    log_assert(game_data_path);

    if (hook_core_piu_block_non_piu_process_recursion(argc, argv)) {
        return;
    }

    hook_core_piu_log_init(argc, argv);

    if (!nxahook_options_init(argc, argv, options)) {
        exit(0);
    }

    hook_core_piu_log_info(argc, argv);

    *game_data_path = hook_core_piu_utils_get_and_verify_game_data_path(argc, argv);

    hook_core_piu_utils_verify_root_user();

    log_info("Initializing, piu game data path: %s", *game_data_path);
}

static void nxahook_patch_fs_init(struct nxahook_options* options)
{
    log_assert(options);

    patch_hook_mon_init(
        options->patch.hook_mon.io,
        options->patch.hook_mon.file,
        options->patch.hook_mon.fs,
        options->patch.hook_mon.usb,
        options->patch.hook_mon.open);

    patch_redir_init();
}

static void nxahook_fs_redir_settings(struct nxahook_options* options)
{
    log_assert(options);

    if (options->game.settings) {
        if (!util_fs_path_exists(options->game.settings)) {
            log_info("Settings path %s does not exist, creating", options->game.settings);

            if (!util_fs_mkdir(options->game.settings)) {
                log_error("Creating directory %s failed", options->game.settings);
            }
        }

        char* abs_path = util_fs_get_abs_path(options->game.settings);

        cnh_redir_add("/SETTINGS", abs_path);
        free(abs_path);
    } else {
        log_warn("No settings path specified");
    }
}

static void nxahook_fs_redir_script(struct nxahook_options* options, const char* game_data_path)
{
    log_assert(options);
    log_assert(game_data_path);

    char* path_tmp;

    path_tmp = util_str_merge(game_data_path, "/SCRIPT");
    cnh_redir_add("/SCRIPT", path_tmp);
    free(path_tmp);
}

static void nxahook_fs_redir_fonts(struct nxahook_options* options, const char* game_data_path)
{
    log_assert(options);
    log_assert(game_data_path);

    char* path_tmp;

    /* Fonts, game expects lower case files */
    path_tmp = util_str_merge(game_data_path, "/nx.ttf");
    cnh_redir_add("nx.ttf", path_tmp);
    free(path_tmp);

    path_tmp = util_str_merge(game_data_path, "/nxcn.ttf");
    cnh_redir_add("nxcn.ttf", path_tmp);
    free(path_tmp);

    path_tmp = util_str_merge(game_data_path, "/nxpt.ttf");
    cnh_redir_add("nxpt.ttf", path_tmp);
    free(path_tmp);

    path_tmp = util_str_merge(game_data_path, "/nxtw.ttf");
    cnh_redir_add("nxtw.ttf", path_tmp);
    free(path_tmp);
}

static void nxahook_fs_redir_mission(struct nxahook_options* options, const char* game_data_path)
{
    log_assert(options);
    log_assert(game_data_path);

    char* path_tmp;

    /* mission.txt, game expects lower case file */
    path_tmp = util_str_merge(game_data_path, "/mission.txt");
    cnh_redir_add("/mission.txt", path_tmp);
    free(path_tmp);

    /* ufo.txt, game expects lower case file */
    path_tmp = util_str_merge(game_data_path, "/ufo.txt");
    cnh_redir_add("/ufo.txt", path_tmp);
    free(path_tmp);
}

static void nxahook_fs_redir_usb()
{
    /* Needed on newer kernels */
    cnh_redir_add("/proc/bus/usb/devices", "/sys/kernel/debug/usb/devices");
    patch_usbfs_init();
}

static void nxahook_fs_redirs_init(struct nxahook_options* options, const char* game_data_path)
{
    log_assert(options);
    log_assert(game_data_path);

    /* Expecting all assets in the piu game (asset) folder. redirects for files
       that are not in there by default for reasons */
    nxahook_fs_redir_settings(options);
    nxahook_fs_redir_script(options, game_data_path);
    nxahook_fs_redir_fonts(options, game_data_path);
    nxahook_fs_redir_mission(options, game_data_path);

    nxahook_fs_redir_usb();
}

static void nxahook_patch_fs_mounting_init()
{
    patch_mounts_init();

    patch_usb_mnt_init();
    patch_usb_unlock_init();
}

static void nxahook_patch_gfx_init(struct nxahook_options* options)
{
    log_assert(options);

    patch_gfx_init();

    if (options->patch.gfx.windowed) {
        patch_gfx_force_window_mode();
    }
}

static void nxahook_patch_main_loop_init(struct nxahook_options* options)
{
    log_assert(options);

    patch_main_loop_init(false, options->patch.main_loop.disable_built_in_inputs, false);

    if (options->patch.main_loop.x11_input_handler_api_lib) {
        char* abs_path_x11_input_hook_lib = util_fs_get_abs_path(options->patch.main_loop.x11_input_handler_api_lib);

        patch_main_loop_add_x11_input_handler(abs_path_x11_input_hook_lib);
        free(abs_path_x11_input_hook_lib);
    }
}

static void nxahook_patch_sound_init(struct nxahook_options* options)
{
    log_assert(options);

    patch_sound_init(options->patch.sound.device);
}

static void nxahook_patch_sigsegv_init(struct nxahook_options* options)
{
    log_assert(options);

    patch_sigsegv_init(options->patch.sigsegv.halt_on_segv);
}

static void nxahook_patch_dongle_init()
{
    patch_microdog40_init((const uint8_t*) _binary_nxahook_dog_key_start,
        ((uintptr_t) &_binary_nxahook_dog_key_end - (uintptr_t) &_binary_nxahook_dog_key_start));
}

static void nxahook_patch_hdd_check_init()
{
    patch_hdd_check_init((const uint8_t*) _binary_nxahook_hdd_raw_start,
        ((uintptr_t) &_binary_nxahook_hdd_raw_end - (uintptr_t) &_binary_nxahook_hdd_raw_start));
}

static void nxahook_patch_piuio_init(struct nxahook_options* options)
{
    log_assert(options);

    /* Hook before IO emulation */
    if (options->patch.piuio.exit_test_serv) {
        patch_piuio_exit_init();
    }

    patch_usb_init_fix_init();
    patch_usb_emu_init();

    if (options->patch.piuio.api_lib) {
        char* abs_path_iolib = util_fs_get_abs_path(options->patch.piuio.api_lib);

        patch_piuio_init(abs_path_iolib);
        free(abs_path_iolib);
    }
}

static void nxahook_patch_profile()
{
    nxahook_profile_gen_init();
}

static void nxahook_patch_pumpnet(struct nxahook_options* options)
{
     if (options->patch.net.server && options->patch.net.machine_id != 0) {
        patch_net_profile_init(
            ASSET_GAME_VERSION_NXA,
            options->patch.net.server,
            options->patch.net.machine_id,
            options->patch.net.cert_dir_path,
            options->patch.net.verbose_log_output);
    }
}

void nxahook_constructor(void)
{
    /* Nothing here */
}

void nxahook_destructor(void)
{
    /* Nothing here */
}

void nxahook_trap_before_main(int argc, char** argv)
{
    log_assert(argc >= 0);
    log_assert(argv);

    char* game_data_path;
    struct nxahook_options options;

    nxahook_bootstrapping(argc, argv, &options, &game_data_path);
    nxahook_patch_fs_init(&options);
    nxahook_fs_redirs_init(&options, game_data_path);
    nxahook_patch_fs_mounting_init();
    nxahook_patch_gfx_init(&options);
    nxahook_patch_main_loop_init(&options);
    nxahook_patch_sound_init(&options);
    nxahook_patch_sigsegv_init(&options);
    nxahook_patch_dongle_init();
    nxahook_patch_hdd_check_init();
    nxahook_patch_piuio_init(&options);
    // Init order of pumpnet and auto profile generating important: pumpnet hook, if active, needs to be applied before
    // the auto gen profile hook. Otherwise, the game detects that no profile is available and auto generates one even
    // it should take the pumpnet route to grab a profile from a remote server
    nxahook_patch_pumpnet(&options);
    nxahook_patch_profile();

    free(game_data_path);

    log_info("Hooking finished");
}

void nxahook_trap_after_main(void)
{
    patch_piuio_shutdown();
}

LIB_MAIN_CONSTRUCTOR(nxahook_constructor);
LIB_MAIN_DESTRUCTOR(nxahook_destructor);
LIB_MAIN_TRAP_MAIN(nxahook_trap_before_main, nxahook_trap_after_main);