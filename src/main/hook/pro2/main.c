/**
 * Hook library for Pump It Up: Pro 2
 */
#define LOG_MODULE "pro2hook"

#include <string.h>

#include "capnhook/hook/lib-main.h"
#include "capnhook/hooklib/redir.h"

#include "hook/core/piu-utils.h"

#include "hook/patch/hook-mon.h"
#include "hook/patch/piuio.h"
#include "hook/patch/piuio-exit.h"
#include "hook/patch/redir.h"
#include "hook/patch/usb-emu.h"
#include "hook/patch/usb-init-fix.h"
#include "hook/patch/x11-event-loop.h"

#include "hook/patch/piubtn.h"
#include "hook/patch/piuio.h"

#include "util/fs.h"
#include "util/glibc.h"
#include "util/log.h"
#include "util/patch.h"
#include "util/sys-info.h"

#include "options.h"

/**
 * Copy/paste from binary:
 *  int PatchOriginalDlopen()
 *  {
 *  int result; // eax@1
 *
 *  result = mprotect((void *)((unsigned int)dlopen & 0xFFFFF000), 0x2000u, 7) + 1;
 *  if ( !result )
 *  {
 *      sub_836EF70("../../src/pump/SecureBinary.cpp", 127, (int)"Assertion 'ret != -1' failed");
 *      sub_821B1B0("Assertion 'ret != -1' failed");
 *  }
 *  if ( *(_BYTE *)dlopen != -1 )
 *  {
 *      sub_836EF70("../../src/pump/SecureBinary.cpp", 130, (int)"Assertion 'pDlopen[0] == 0xFF' failed");
 *      sub_821B1B0("Assertion 'pDlopen[0] == 0xFF' failed");
 *  }
 *  *((_BYTE *)dlopen + 5) = 0x90u;
 *  *(_BYTE *)dlopen = 0xE9u;
 *  dword_865C03C = *(_DWORD *)dlopen;
 *  word_865C040 = *((_WORD *)dlopen + 2);
 *  *(_DWORD *)((char *)dlopen + 1) = (char *)sub_8383C90 + 0xF7FB232B;
 *  return result;
 *   }
 */
static void pro2hook_patch_secure_binary_dlopen_switcheroo()
{
    uint8_t buffer[4] = {0xC3, 0x90, 0x90, 0x90};

    log_info("Patching dlopen switcheroo");

    /* This address will only work with the latest revision executable (iirc
       that was R5?) */
    util_patch_write_memory(0x8383BC0, buffer, 4);
}

static void pro2hook_bootstrapping(int argc, char** argv, struct pro2hook_options* options)
{
    log_assert(argc >= 0);
    log_assert(argv);
    log_assert(options);

    if (hook_core_piu_block_non_piu_process_recursion(argc, argv)) {
        return;
    }

    hook_core_piu_log_init(argc, argv);

    if (!pro2hook_options_init(argc, argv, options)) {
        exit(0);
    }

    hook_core_piu_log_info(argc, argv);

    hook_core_piu_utils_verify_root_user();

    log_info("Initializing");
}

static void pro2hook_patch_fs_init(struct pro2hook_options* options)
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

static void pro2hook_fs_redir_data(struct pro2hook_options* options)
{
    log_assert(options);

    if (options->game.data) {
        char* abs_path = util_fs_get_abs_path(options->game.data);

        cnh_redir_add("/pro2", abs_path);
        free(abs_path);
    } else {
        log_warn("No data (game) path specified");
    }
}

static void pro2hook_fs_redirs_init(struct pro2hook_options* options)
{
    log_assert(options);

    pro2hook_fs_redir_data(options);
}

static void pro2hook_patch_sigsegv_init(struct pro2hook_options* options)
{
    log_assert(options);

    /* seems like pump pro doesn't like it when injecting our own signal
       handler. game crashes instantly when adding it. */
    /*
    patch_sigsegv_init();
    */
}

static void pro2hook_patch_x11_event_loop_init(struct pro2hook_options* options)
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

static void pro2hook_patch_piuio_init(struct pro2hook_options* options)
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

static void pro2hook_patch_piubtn_init(struct pro2hook_options* options)
{
    if (options->patch.piubtn.api_lib) {
        char* abs_path_btnlib = util_fs_get_abs_path(options->patch.piubtn.api_lib);

        patch_piubtn_init(abs_path_btnlib);
        free(abs_path_btnlib);
    }
}

static void pro2hook_patch_secure_binary()
{
    pro2hook_patch_secure_binary_dlopen_switcheroo();
}

void pro2hook_constructor(void)
{
    /* Nothing here */
}

void pro2hook_destructor(void)
{
    /* Nothing here */
}

void pro2hook_trap_before_main(int argc, char** argv)
{
    log_assert(argc >= 0);
    log_assert(argv);

    struct pro2hook_options options;

    pro2hook_bootstrapping(argc, argv, &options);
    pro2hook_patch_fs_init(&options);
    pro2hook_fs_redirs_init(&options);
    pro2hook_patch_sigsegv_init(&options);
    pro2hook_patch_x11_event_loop_init(&options);
    pro2hook_patch_piuio_init(&options);
    pro2hook_patch_piubtn_init(&options);
    pro2hook_patch_secure_binary();

    log_info("Hooking finished");
}

void pro2hook_trap_after_main(void)
{
    patch_piubtn_shutdown();
    patch_piuio_shutdown();
}

LIB_MAIN_CONSTRUCTOR(pro2hook_constructor);
LIB_MAIN_DESTRUCTOR(pro2hook_destructor);
LIB_MAIN_TRAP_MAIN(pro2hook_trap_before_main, pro2hook_trap_after_main);