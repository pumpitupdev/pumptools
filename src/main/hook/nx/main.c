/**
 * Hook library for Pump It Up: NX
 */
#define LOG_MODULE "nxhook"

#include <string.h>

#include "capnhook/hook/lib-main.h"
#include "capnhook/hooklib/redir.h"

#include "hook/core/piu-utils.h"

#include "hook/patch/gfx.h"
#include "hook/patch/hdd-check.h"
#include "hook/patch/hook-mon.h"
#include "hook/patch/main-loop.h"
#include "hook/patch/microdog34.h"
#include "hook/patch/piuio-exit.h"
#include "hook/patch/piuio.h"
#include "hook/patch/redir.h"
#include "hook/patch/sigsegv.h"
#include "hook/patch/sound.h"
#include "hook/patch/usb-emu.h"
#include "hook/patch/usb-init-fix.h"

#include "util/fs.h"
#include "util/glibc.h"
#include "util/str.h"
#include "util/sys-info.h"

#include "options.h"
#include "unlock.h"

/* Compiled binary data from data folder. Symbols are defined by compiler */
extern const uint8_t _binary_nxhook_hdd_raw_start[];
extern const uint8_t _binary_nxhook_hdd_raw_end[];
extern const uint8_t _binary_nxhook_dog_key_start[];
extern const uint8_t _binary_nxhook_dog_key_end[];

static void nxhook_bootstrapping(
    int argc,
    char **argv,
    struct nxhook_options *options,
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

  if (!nxhook_options_init(argc, argv, options)) {
    exit(0);
  }

  hook_core_piu_log_info(argc, argv);

  *game_data_path =
      hook_core_piu_utils_get_and_verify_game_data_path(argc, argv);

  hook_core_piu_utils_verify_root_user();

  log_info("Initializing, piu game data path: %s", *game_data_path);
}

static void nxhook_patch_fs_init(struct nxhook_options *options)
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

static void nxhook_fs_redir_settings(struct nxhook_options *options)
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

static void nxhook_fs_redir_script(
    struct nxhook_options *options, const char *game_data_path)
{
  log_assert(options);
  log_assert(game_data_path);

  char *path_tmp;

  path_tmp = util_str_merge(game_data_path, "/SCRIPT");
  cnh_redir_add("/SCRIPT", path_tmp);
  free(path_tmp);
}

static void nxhook_fs_redir_fonts(
    struct nxhook_options *options, const char *game_data_path)
{
  log_assert(options);
  log_assert(game_data_path);

  char *path_tmp;

  path_tmp = util_str_merge(game_data_path, "/MICROGBE.TTF");
  cnh_redir_add("MICROGBE.TTF", path_tmp);
  free(path_tmp);

  path_tmp = util_str_merge(game_data_path, "/NXTW.TTF");
  cnh_redir_add("NXTW.TTF", path_tmp);
  free(path_tmp);
}

static void nxhook_fs_redir_usb()
{
  /* Needed on newer kernels */
  cnh_redir_add("/proc/bus/usb/devices", "/sys/kernel/debug/usb/devices");
}

static void nxhook_fs_redirs_init(
    struct nxhook_options *options, const char *game_data_path)
{
  log_assert(options);
  log_assert(game_data_path);

  /* Expecting all assets in the piu game (asset) folder. redirects for files
     that are not in there by default for reasons */
  nxhook_fs_redir_settings(options);
  nxhook_fs_redir_script(options, game_data_path);
  nxhook_fs_redir_fonts(options, game_data_path);

  nxhook_fs_redir_usb();
}

static void nxhook_patch_gfx_init(struct nxhook_options *options)
{
  log_assert(options);

  patch_gfx_init();

  if (options->patch.gfx.scaling_mode != PATCH_GFX_SCALE_MODE_INVALID) {
    patch_gfx_scale(options->patch.gfx.scaling_mode);
  }
}

static void nxhook_patch_game_init(struct nxhook_options *options)
{
  log_assert(options);

  if (options->game.force_unlock) {
    hook_nx_unlock_init();
  }
}

static void nxhook_patch_main_loop_init(struct nxhook_options *options)
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

static void nxhook_patch_sound_init(struct nxhook_options *options)
{
  log_assert(options);

  patch_sound_init(options->patch.sound.device);
}

static void nxhook_patch_sigsegv_init(struct nxhook_options *options)
{
  log_assert(options);

  patch_sigsegv_init(options->patch.sigsegv.halt_on_segv);
}

static void nxhook_patch_dongle_init()
{
  patch_microdog34_init(
      (const uint8_t *) _binary_nxhook_dog_key_start,
      ((uintptr_t) &_binary_nxhook_dog_key_end -
       (uintptr_t) &_binary_nxhook_dog_key_start));
}

static void nxhook_patch_hdd_check_init()
{
  patch_hdd_check_init(
      (const uint8_t *) _binary_nxhook_hdd_raw_start,
      ((uintptr_t) &_binary_nxhook_hdd_raw_end -
       (uintptr_t) &_binary_nxhook_hdd_raw_start));
}

static void nxhook_patch_piuio_init(struct nxhook_options *options)
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

void nxhook_constructor(void)
{
  /* Nothing here */
}

void nxhook_destructor(void)
{
  /* Nothing here */
}

void nxhook_trap_before_main(int argc, char **argv)
{
  log_assert(argc >= 0);
  log_assert(argv);

  char *game_data_path;
  struct nxhook_options options;

  nxhook_bootstrapping(argc, argv, &options, &game_data_path);
  nxhook_patch_fs_init(&options);
  nxhook_fs_redirs_init(&options, game_data_path);
  nxhook_patch_gfx_init(&options);
  nxhook_patch_game_init(&options);
  nxhook_patch_main_loop_init(&options);
  nxhook_patch_sound_init(&options);
  nxhook_patch_sigsegv_init(&options);
  nxhook_patch_dongle_init();
  nxhook_patch_hdd_check_init();
  nxhook_patch_piuio_init(&options);

  free(game_data_path);

  log_info("Hooking finished");
}

void nxhook_trap_after_main(void)
{
  patch_piuio_shutdown();
}

LIB_MAIN_CONSTRUCTOR(nxhook_constructor);
LIB_MAIN_DESTRUCTOR(nxhook_destructor);
LIB_MAIN_TRAP_MAIN(nxhook_trap_before_main, nxhook_trap_after_main);