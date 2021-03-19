/**
 * Hook library for Pump It Up: Exceed
 */
#define LOG_MODULE "exchook"

#include <string.h>

#include "capnhook/hook/lib-main.h"
#include "capnhook/hooklib/redir.h"

#include "hook/core/piu-utils.h"

#include "hook/patch/asound-fix.h"
#include "hook/patch/gfx.h"
#include "hook/patch/hook-mon.h"
#include "hook/patch/main-loop.h"
#include "hook/patch/redir.h"
#include "hook/patch/sigsegv.h"
#include "hook/patch/sound.h"

#include "util/fs.h"
#include "util/glibc.h"
#include "util/patch.h"
#include "util/str.h"
#include "util/sys-info.h"

#include "eeprom.h"
#include "io.h"
#include "lockchip.h"
#include "mempatch.h"
#include "options.h"

static void exchook_bootstrapping(
    int argc,
    char **argv,
    struct exchook_options *options,
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

  if (!exchook_options_init(argc, argv, options)) {
    exit(0);
  }

  hook_core_piu_log_info(argc, argv);

  *game_data_path =
      hook_core_piu_utils_get_and_verify_game_data_path(argc, argv);

  hook_core_piu_utils_verify_root_user();

  log_info("Initializing, piu game data path: %s", *game_data_path);
}

static void exchook_game_version_mempatch_table_init(
    struct exchook_options *options,
    const struct exchook_mempatch_table **patch_table)
{
  log_assert(options);
  log_assert(patch_table);

  if (!options->game.version) {
    log_die("No game version specified");
  } else {
    log_info("Game version configured: %s", options->game.version);
  }

  *patch_table = exchook_mempatch_get_table(options->game.version);

  if (*patch_table == NULL) {
    log_die("Unsupported game version %s", options->game.version);
  }
}

static void exchook_patch_fs_init(struct exchook_options *options)
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

static void exchook_fs_redir_settings(struct exchook_options *options)
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

static void exchook_fs_redir_usb()
{
  /* Needed on newer kernels */
  cnh_redir_add("/proc/bus/usb/devices", "/sys/kernel/debug/usb/devices");
}

static void exchook_patch_gfx_init(struct exchook_options *options)
{
  log_assert(options);

  patch_gfx_init();

  if (options->patch.gfx.windowed) {
    patch_gfx_force_window_mode();
  }
}

static void exchook_patch_main_loop_init(struct exchook_options *options)
{
  log_assert(options);

  patch_main_loop_init(
      true, options->patch.main_loop.disable_built_in_inputs, false);

  if (options->patch.main_loop.x11_input_handler_api_lib) {
    char *abs_path_x11_input_hook_lib = util_fs_get_abs_path(
        options->patch.main_loop.x11_input_handler_api_lib);

    patch_main_loop_add_x11_input_handler(abs_path_x11_input_hook_lib);
    free(abs_path_x11_input_hook_lib);
  }
}

static void exchook_patch_sound_init(
    struct exchook_options *options,
    const struct exchook_mempatch_table *patch_table)
{
  log_assert(options);
  log_assert(patch_table);

  /* exceed crashes with its default device hw:0. use dmix instead */
  if (strcmp(options->patch.sound.device, "dmix") != 0) {
    log_warn(
        "Game might crash with selected sound device %s, use 'dmix' instead if "
        "that happens",
        options->patch.sound.device);
  }

  patch_sound_init(options->patch.sound.device);
  patch_asound_fix_init();

  /* return from StopAllEffects to avoid killing the sound thread */
  util_patch_write_memory_byte(patch_table->addr_stop_all_effects_return, 0xC3);
}

static void exchook_patch_sigsegv_init(struct exchook_options *options)
{
  log_assert(options);

  patch_sigsegv_init(options->patch.sigsegv.halt_on_segv);
}

static void
exchook_patch_lockchip_init(const struct exchook_mempatch_table *patch_table)
{
  log_assert(patch_table);

  exchook_lockchip_init(patch_table);
}

static void exchook_patch_eeprom_init(
    struct exchook_options *options,
    const struct exchook_mempatch_table *patch_table)
{
  log_assert(options);
  log_assert(patch_table);

  char *eeprom_path;

  if (options->game.settings) {
    char *abs_path = util_fs_get_abs_path(options->game.settings);

    eeprom_path = util_str_merge(abs_path, "/eeprom.bin");
    free(abs_path);
  } else {
    eeprom_path = util_str_dup("/tmp/eeprom.bin");
    log_warn(
        "No settings path specified to store eeprom data, defaulting to %s",
        eeprom_path);
  }

  exchook_eeprom_init(patch_table, eeprom_path);
  free(eeprom_path);
}

static void exchook_patch_piuio_init(
    struct exchook_options *options,
    const struct exchook_mempatch_table *patch_table)
{
  log_assert(options);
  log_assert(patch_table);

  if (options->patch.piuio.api_lib) {
    char *abs_path_iolib = util_fs_get_abs_path(options->patch.piuio.api_lib);

    exchook_io_init(
        patch_table, abs_path_iolib, options->patch.piuio.exit_test_serv);
    free(abs_path_iolib);
  }
}

void exchook_constructor(void)
{
  /* Nothing here */
}

void exchook_destructor(void)
{
  /* Nothing here */
}

void exchook_trap_before_main(int argc, char **argv)
{
  log_assert(argc >= 0);
  log_assert(argv);

  char *game_data_path;
  struct exchook_options options;
  const struct exchook_mempatch_table *mempatch_table;

  exchook_bootstrapping(argc, argv, &options, &game_data_path);
  exchook_game_version_mempatch_table_init(&options, &mempatch_table);
  exchook_patch_fs_init(&options);
  exchook_fs_redir_settings(&options);
  exchook_fs_redir_usb();
  exchook_patch_gfx_init(&options);
  exchook_patch_main_loop_init(&options);
  exchook_patch_sound_init(&options, mempatch_table);
  exchook_patch_sigsegv_init(&options);
  exchook_patch_lockchip_init(mempatch_table);
  exchook_patch_eeprom_init(&options, mempatch_table);
  exchook_patch_piuio_init(&options, mempatch_table);

  free(game_data_path);

  log_info("Hooking finished");
}

void exchook_trap_after_main(void)
{
  exchook_io_shutdown();
}

LIB_MAIN_CONSTRUCTOR(exchook_constructor);
LIB_MAIN_DESTRUCTOR(exchook_destructor);
LIB_MAIN_TRAP_MAIN(exchook_trap_before_main, exchook_trap_after_main);