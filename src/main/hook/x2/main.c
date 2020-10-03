/**
 * Hook library for Pump It Up: Exceed 2
 */
#define LOG_MODULE "x2hook"

#include <string.h>

#include "capnhook/hook/lib-main.h"
#include "capnhook/hooklib/redir.h"

#include "hook/core/piu-utils.h"

#include "hook/patch/gfx.h"
#include "hook/patch/hook-mon.h"
#include "hook/patch/main-loop.h"
#include "hook/patch/microdog34.h"
#include "hook/patch/piuio.h"
#include "hook/patch/piuio-exit.h"
#include "hook/patch/redir.h"
#include "hook/patch/sigsegv.h"
#include "hook/patch/sound.h"
#include "hook/patch/usb-emu.h"
#include "hook/patch/usb-init-fix.h"

#include "util/fs.h"
#include "util/glibc.h"
#include "util/patch.h"
#include "util/str.h"
#include "util/sys-info.h"

#include "options.h"
#include "unlock.h"

/* Compiled binary data from data folder. Symbols are defined by compiler */
extern const uint8_t _binary_x2hook_dog_key_start[];
extern const uint8_t _binary_x2hook_dog_key_end[];

static void x2hook_bootstrapping(int argc, char** argv, struct x2hook_options* options, char** game_data_path)
{
    log_assert(argc >= 0);
    log_assert(argv);
    log_assert(options);
    log_assert(game_data_path);

    if (hook_core_piu_block_non_piu_process_recursion(argc, argv)) {
        return;
    }

    hook_core_piu_log_init(argc, argv);

    if (!x2hook_options_init(argc, argv, options)) {
        exit(0);
    }

    hook_core_piu_log_info(argc, argv);

    *game_data_path = hook_core_piu_utils_get_and_verify_game_data_path(argc, argv);

    hook_core_piu_utils_verify_root_user();

    log_info("Initializing, piu game data path: %s", *game_data_path);
}

static void x2hook_patch_fs_init(struct x2hook_options* options)
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

static void x2hook_fs_redir_settings(struct x2hook_options* options)
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

static void x2hook_fs_redir_script(struct x2hook_options* options, const char* game_data_path)
{
    log_assert(options);
    log_assert(game_data_path);

    char* path_tmp;

    path_tmp = util_str_merge(game_data_path, "/SCRIPT");
    cnh_redir_add("/SCRIPT", path_tmp);
    free(path_tmp);
}

static void x2hook_fs_redir_usb()
{
    /* Needed on newer kernels */
    cnh_redir_add("/proc/bus/usb/devices", "/sys/kernel/debug/usb/devices");
}

static void x2hook_fs_redirs_init(struct x2hook_options* options, const char* game_data_path)
{
    log_assert(options);
    log_assert(game_data_path);

    /* Expecting all assets in the piu game (asset) folder. redirects for files
       that are not in there by default for reasons */
    x2hook_fs_redir_settings(options);
    x2hook_fs_redir_script(options, game_data_path);

    x2hook_fs_redir_usb();
}

static void x2hook_patch_gfx_init(struct x2hook_options* options)
{
    log_assert(options);

    patch_gfx_init();

    if (options->patch.gfx.windowed) {
        patch_gfx_force_window_mode();
    }
}

static void x2hook_patch_game_init(struct x2hook_options* options)
{
    log_assert(options);

    if (options->game.force_unlock) {
        hook_x2_unlock_init();
    }

    /* return from StopAllEffects to avoid killing the sound thread */
    log_warn("Applying memory patch only valid for version 102");
    util_patch_write_memory_byte(0x08074CE0, 0xC3);
}

static void x2hook_patch_main_loop_init(struct x2hook_options* options)
{
    log_assert(options);

    patch_main_loop_init(true, options->patch.main_loop.disable_built_in_inputs, false);

    if (options->patch.main_loop.x11_input_handler_api_lib) {
        char* abs_path_x11_input_hook_lib = util_fs_get_abs_path(options->patch.main_loop.x11_input_handler_api_lib);

        patch_main_loop_add_x11_input_handler(abs_path_x11_input_hook_lib);
        free(abs_path_x11_input_hook_lib);
    }
}

static void x2hook_patch_sound_init(struct x2hook_options* options)
{
    log_assert(options);

    patch_sound_init(options->patch.sound.device);
}

static void x2hook_patch_sigsegv_init(struct x2hook_options* options)
{
    log_assert(options);

    patch_sigsegv_init(options->patch.sigsegv.halt_on_segv);
}

static void x2hook_patch_dongle_init()
{
    patch_microdog34_init((const uint8_t*) _binary_x2hook_dog_key_start,
        ((uintptr_t) &_binary_x2hook_dog_key_end - (uintptr_t) &_binary_x2hook_dog_key_start));
}

static void x2hook_patch_piuio_init(struct x2hook_options* options)
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

void x2hook_constructor(void)
{
    /* Nothing here */
}

void x2hook_destructor(void)
{
    /* Nothing here */
}

void x2hook_trap_before_main(int argc, char** argv)
{
    log_assert(argc >= 0);
    log_assert(argv);

    char* game_data_path;
    struct x2hook_options options;

    x2hook_bootstrapping(argc, argv, &options, &game_data_path);
    x2hook_patch_fs_init(&options);
    x2hook_fs_redirs_init(&options, game_data_path);
    x2hook_patch_gfx_init(&options);
    x2hook_patch_game_init(&options);
    x2hook_patch_main_loop_init(&options);
    x2hook_patch_sound_init(&options);
    x2hook_patch_sigsegv_init(&options);
    x2hook_patch_dongle_init();
    x2hook_patch_piuio_init(&options);

    free(game_data_path);

    log_info("Hooking finished");
}

void x2hook_trap_after_main(void)
{
    patch_piuio_shutdown();
}

LIB_MAIN_CONSTRUCTOR(x2hook_constructor);
LIB_MAIN_DESTRUCTOR(x2hook_destructor);
LIB_MAIN_TRAP_MAIN(x2hook_trap_before_main, x2hook_trap_after_main);