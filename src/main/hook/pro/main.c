/**
 * Hook library for Pump It Up: Pro
 */
#define LOG_MODULE "prohook"

#include <string.h>

#include "capnhook/hook/lib-main.h"
#include "capnhook/hooklib/redir.h"

#include "hook/core/piu-utils.h"

#include "hook/patch/hook-mon.h"
#include "hook/patch/piuio.h"
#include "hook/patch/piuio-exit.h"
#include "hook/patch/piuio-khack.h"
#include "hook/patch/redir.h"
#include "hook/patch/usb-emu.h"
#include "hook/patch/usb-init-fix.h"
#include "hook/patch/x11-event-loop.h"

#include "hook/propatch/usb-fix.h"

#include "hook/patch/piubtn.h"
#include "hook/patch/piuio.h"

#include "util/fs.h"
#include "util/glibc.h"
#include "util/log.h"
#include "util/sys-info.h"

#include "options.h"

static void prohook_bootstrapping(int argc, char** argv, struct prohook_options* options)
{
    log_assert(argc >= 0);
    log_assert(argv);
    log_assert(options);

    if (hook_core_piu_block_non_piu_process_recursion(argc, argv)) {
        return;
    }

    hook_core_piu_log_init(argc, argv);

    if (!prohook_options_init(argc, argv, options)) {
        exit(0);
    }

    hook_core_piu_log_info(argc, argv);

    hook_core_piu_utils_verify_root_user();

    log_info("Initializing");
}

static void prohook_patch_fs_init(struct prohook_options* options)
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

static void prohook_fs_redir_settings(struct prohook_options* options)
{
    if (options->game.settings) {
        if (!util_fs_path_exists(options->game.settings)) {
            log_info("Settings path %s does not exist, creating", options->game.settings);

            if (!util_fs_mkdir(options->game.settings)) {
                log_error("Creating directory %s failed", options->game.settings);
            }
        }

        char* abs_path = util_fs_get_abs_path(options->game.settings);

        cnh_redir_add("/data", abs_path);
        free(abs_path);
    } else {
        log_warn("No settings (data) path specified");
    }
}

static void prohook_fs_redir_data(struct prohook_options* options)
{
    if (options->game.data) {
        char* abs_path = util_fs_get_abs_path(options->game.data);

        cnh_redir_add("/game", abs_path);
        free(abs_path);
    } else {
        log_warn("No data (game) path specified");
    }
}

static void prohook_fs_redirs_init(struct prohook_options* options)
{
    log_assert(options);

    prohook_fs_redir_settings(options);
    prohook_fs_redir_data(options);
}

static void prohook_patch_sigsegv_init(struct prohook_options* options)
{
    log_assert(options);

    /* seems like pump pro doesn't like it when injecting our own signal
       handler. game crashes instantly when adding it. */
    /*
    patch_sigsegv_init();
    */
}

static void prohook_patch_x11_event_loop_init(struct prohook_options* options)
{
    patch_x11_event_loop_init();

    // Register up to two X11 input handlers for piuio and piubtn

    if (options->patch.x11_event_loop.api_lib) {
        char* abs_path_lib = util_fs_get_abs_path(options->patch.x11_event_loop.api_lib);

        patch_x11_event_loop_add_input_handler(abs_path_lib);
        free(abs_path_lib);
    }

    if (options->patch.x11_event_loop.api_lib2) {
        char* abs_path_lib = util_fs_get_abs_path(options->patch.x11_event_loop.api_lib2);

        patch_x11_event_loop_add_input_handler(abs_path_lib);
        free(abs_path_lib);
    }
}

static void prohook_patch_usbprofiles(struct prohook_options* options)
{
    propatch_usb_fix_init(
        options->patch.usb_profile.device_nodes,
        options->patch.usb_profile.p1_bus_port,
        options->patch.usb_profile.p2_bus_port);
}

static void prohook_patch_piuio_init(struct prohook_options* options)
{
    log_assert(options);

    // The order of layering the hooks is important here because of deps
    // 1. Fix low level libusb thing with init
    // 2. Introduce the shim layer to allow adding fakedevs required for forther software emulation
    // 3. Turn the ITG 2 piuio kernel hack calls into normal piuio ctrl transfer calls
    // 4. Exit module to allow exiting the game with test + service
    // 5. Software emulation layer for piuio
    //
    // For example, the kernel hack layer takes care of removing that, so software
    // emulation code doesn't have to bother with that edge case. Furthermore, it allows you
    // to use the game with real hardware without any additional software emulation layers
    // that have to detour to real hardware again

    patch_usb_init_fix_init();
    patch_usb_emu_init();
    patch_piuio_khack_init();

    /* Hook before PIUIO emulation */
    if (options->patch.piuio.exit_test_serv) {
        patch_piuio_exit_init();
    }

    if (options->patch.piuio.api_lib) {
        char* abs_path_iolib = util_fs_get_abs_path(options->patch.piuio.api_lib);

        patch_piuio_init(abs_path_iolib);
        free(abs_path_iolib);
    }
}

static void prohook_patch_piubtn_init(struct prohook_options* options)
{
    if (options->patch.piubtn.api_lib) {
        char* abs_path_btnlib = util_fs_get_abs_path(options->patch.piubtn.api_lib);

        patch_piubtn_init(abs_path_btnlib);
        free(abs_path_btnlib);
    }
}

void prohook_constructor(void)
{
    /* Nothing here */
}

void prohook_destructor(void)
{
    /* Nothing here */
}

void prohook_trap_before_main(int argc, char** argv)
{
    log_assert(argc >= 0);
    log_assert(argv);

    struct prohook_options options;

    prohook_bootstrapping(argc, argv, &options);
    prohook_patch_fs_init(&options);
    prohook_fs_redirs_init(&options);
    prohook_patch_sigsegv_init(&options);
    prohook_patch_x11_event_loop_init(&options);
    prohook_patch_usbprofiles(&options);
    prohook_patch_piuio_init(&options);
    prohook_patch_piubtn_init(&options);

    log_info("Hooking finished");
}

void prohook_trap_after_main(void)
{
    propatch_usb_fix_shutdown();
    patch_piubtn_shutdown();
    patch_piuio_shutdown();
}

LIB_MAIN_CONSTRUCTOR(prohook_constructor);
LIB_MAIN_DESTRUCTOR(prohook_destructor);
LIB_MAIN_TRAP_MAIN(prohook_trap_before_main, prohook_trap_after_main);