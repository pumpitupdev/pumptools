/**
 * Hook library for Pump It Up: Prime
 */
#define LOG_MODULE "prihook"

#include <string.h>

#include "capnhook/hook/lib-main.h"
#include "capnhook/hooklib/redir.h"

#include "hook/core/piu-utils.h"

#include "hook/patch/amixer-block.h"
#include "hook/patch/blacklist-url.h"
#include "hook/patch/block-keyboard-grab.h"
#include "hook/patch/gfx.h"
#include "hook/patch/hasp.h"
#include "hook/patch/hdd-check.h"
#include "hook/patch/hook-mon.h"
#include "hook/patch/main-loop.h"
#include "hook/patch/mounts.h"
#include "hook/patch/network.h"
#include "hook/patch/piuio-exit.h"
#include "hook/patch/piuio.h"
#include "hook/patch/ram-wipe.h"
#include "hook/patch/redir.h"
#include "hook/patch/sigsegv.h"
#include "hook/patch/sound.h"
#include "hook/patch/usb-emu.h"
#include "hook/patch/usb-init-fix.h"
#include "hook/patch/usb-mnt.h"
#include "hook/patch/usbfs.h"

#include "util/fs.h"
#include "util/glibc.h"
#include "util/net.h"
#include "util/sys-info.h"

#include "options.h"
#include "usb-updates.h"

/* Compiled binary data from data folder. Symbols are defined by compiler */
extern const uint8_t _binary_prihook_hdd_raw_start[];
extern const uint8_t _binary_prihook_hdd_raw_end[];
extern const uint8_t _binary_prihook_hasp_key_start[];
extern const uint8_t _binary_prihook_hasp_key_end[];

static void prihook_bootstrapping(
    int argc,
    char **argv,
    struct prihook_options *options,
    char **game_data_path)
{
  log_assert(argc >= 0);
  log_assert(argv);
  log_assert(options);
  log_assert(game_data_path);

  if (hook_core_piu_block_non_piu_process_recursion(argc, argv)) {
    return;
  }

  hook_core_piu_log_init(argc, argv);

  if (!prihook_options_init(argc, argv, options)) {
    exit(0);
  }

  hook_core_piu_log_info(argc, argv);

  *game_data_path =
      hook_core_piu_utils_get_and_verify_game_data_path(argc, argv);

  hook_core_piu_utils_verify_root_user();

  hook_core_piu_utils_fix_locale();

  log_info("Initializing, piu game data path: %s", *game_data_path);
}

static void prihook_patch_fs_init(struct prihook_options *options)
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

static void prihook_fs_redir_settings(struct prihook_options *options)
{
  log_assert(options);

  if (options->game.settings) {
    if (!util_fs_path_exists(options->game.settings)) {
      log_info(
          "Settings path %s does not exist, creating", options->game.settings);

      if (!util_fs_mkdir(options->game.settings)) {
        log_error("Creating directory %s failed", options->game.settings);
      }
    }

    char *abs_path = util_fs_get_abs_path(options->game.settings);

    cnh_redir_add("/SETTINGS", abs_path);
    free(abs_path);
  } else {
    log_warn("No settings path specified");
  }
}

static void prihook_fs_redir_usb()
{
  /* Needed on newer kernels */
  cnh_redir_add("/proc/bus/usb/devices", "/sys/kernel/debug/usb/devices");
  patch_usbfs_init();
}

static void prihook_fs_redirs_init(
    struct prihook_options *options, const char *game_data_path)
{
  log_assert(options);
  log_assert(game_data_path);

  prihook_fs_redir_settings(options);

  prihook_fs_redir_usb();
}

static void prihook_patch_fs_mounting_init()
{
  patch_mounts_init();

  patch_usb_mnt_init();
}

static void prihook_patch_gfx_init(struct prihook_options *options)
{
  log_assert(options);

  patch_gfx_init();

  if (options->patch.gfx.scaling_mode != PATCH_GFX_SCALE_MODE_INVALID) {
    patch_gfx_scale(options->patch.gfx.scaling_mode);
  }
}

static void prihook_patch_main_loop_init(struct prihook_options *options)
{
  log_assert(options);

  patch_main_loop_init(
      false, options->patch.main_loop.disable_built_in_inputs, false);

  if (options->patch.main_loop.x11_input_handler_api_lib) {
    char *abs_path_x11_input_hook_lib = util_fs_get_abs_path(
        options->patch.main_loop.x11_input_handler_api_lib);

    patch_main_loop_add_x11_input_handler(abs_path_x11_input_hook_lib);
    free(abs_path_x11_input_hook_lib);
  }

  patch_block_keyboard_grab_init();
}

static void prihook_patch_sound_init(struct prihook_options *options)
{
  log_assert(options);

  patch_sound_init(options->patch.sound.device);
  patch_axmier_block_init();
}

static void prihook_patch_sigsegv_init(struct prihook_options *options)
{
  log_assert(options);

  patch_sigsegv_init(options->patch.sigsegv.halt_on_segv);
}

static void prihook_patch_dongle_init()
{
  patch_hasp_init(
      (const uint8_t *) _binary_prihook_hasp_key_start,
      ((uintptr_t) &_binary_prihook_hasp_key_end -
       (uintptr_t) &_binary_prihook_hasp_key_start));
}

static void prihook_patch_hdd_check_init()
{
  patch_hdd_check_init(
      (const uint8_t *) _binary_prihook_hdd_raw_start,
      ((uintptr_t) &_binary_prihook_hdd_raw_end -
       (uintptr_t) &_binary_prihook_hdd_raw_start));
}

static void prihook_patch_ram_wipe_init()
{
  patch_ram_wipe_init();
}

static void prihook_patch_piuio_init(struct prihook_options *options)
{
  log_assert(options);

  /* Hook before IO emulation */
  if (options->patch.piuio.exit_test_serv) {
    patch_piuio_exit_init();
  }

  patch_usb_init_fix_init();
  patch_usb_emu_init();

  if (options->patch.piuio.api_lib) {
    char *abs_path_iolib = util_fs_get_abs_path(options->patch.piuio.api_lib);

    patch_piuio_init(abs_path_iolib, 0);
    free(abs_path_iolib);
  }
}

static void prihook_patch_net_init(struct prihook_options *options)
{
  patch_blacklist_url_init();
  /* update server */
  patch_blacklist_url_add("http://update_kr");
  /* game server */
  patch_blacklist_url_add("http://www2.piugame.com");

  patch_network_init();

  if (options->patch.net.server) {
    char *address;
    uint16_t port;

    if (!util_net_split_ipv4_str(options->patch.net.server, &address, &port)) {
      log_warn("Invalid format for option network address specified, ignored");
    } else {
      patch_network_redirect_server_address(
          "115.68.108.183", address, (uint16_t) port);
      free(address);
    }
  } else {
    // Redirect to avoid phoning home if no server address is set
    patch_network_redirect_server_address("115.68.108.183", "127.0.0.1", 12345);
  }

  // Idk what this address is...redirect to something save for now
  patch_network_redirect_server_address("45.76.222.114", "127.0.0.1", 12345);
}

static void prihook_patch_usb_init()
{
  prihook_usb_updates_init();
}

void prihook_constructor(void)
{
  /* Nothing here */
}

void prihook_destructor(void)
{
  /* Nothing here */
}

void prihook_trap_before_main(int argc, char **argv)
{
  log_assert(argc >= 0);
  log_assert(argv);

  char *game_data_path;
  struct prihook_options options;

  prihook_bootstrapping(argc, argv, &options, &game_data_path);
  prihook_patch_fs_init(&options);
  prihook_fs_redirs_init(&options, game_data_path);
  prihook_patch_fs_mounting_init();
  prihook_patch_gfx_init(&options);
  prihook_patch_main_loop_init(&options);
  prihook_patch_sound_init(&options);
  prihook_patch_sigsegv_init(&options);
  prihook_patch_dongle_init();
  prihook_patch_hdd_check_init();
  prihook_patch_ram_wipe_init();
  prihook_patch_piuio_init(&options);
  prihook_patch_net_init(&options);
  prihook_patch_usb_init();

  free(game_data_path);

  log_info("Hooking finished");
}

void prihook_trap_after_main(void)
{
  patch_piuio_shutdown();
}

LIB_MAIN_CONSTRUCTOR(prihook_constructor);
LIB_MAIN_DESTRUCTOR(prihook_destructor);
LIB_MAIN_TRAP_MAIN(prihook_trap_before_main, prihook_trap_after_main);