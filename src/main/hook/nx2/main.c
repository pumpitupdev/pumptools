#define LOG_MODULE "nx2hook"

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
#include "hook/patch/piuio-exit.h"
#include "hook/patch/piuio.h"
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
extern const uint8_t _binary_nx2hook_hdd_raw_start[];
extern const uint8_t _binary_nx2hook_hdd_raw_end[];
extern const uint8_t _binary_nx2hook_dog_key_start[];
extern const uint8_t _binary_nx2hook_dog_key_end[];

static void nx2hook_bootstrapping(
    int argc,
    char **argv,
    struct nx2hook_options *options,
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

  if (!nx2hook_options_init(argc, argv, options)) {
    exit(0);
  }

  hook_core_piu_log_info(argc, argv);

  *game_data_path =
      hook_core_piu_utils_get_and_verify_game_data_path(argc, argv);

  hook_core_piu_utils_verify_root_user();

  log_info("Initializing, piu game data path: %s", *game_data_path);
}

static void nx2hook_patch_fs_init(struct nx2hook_options *options)
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

static void nx2hook_fs_redir_settings(struct nx2hook_options *options)
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

static void nx2hook_fs_redir_script(
    struct nx2hook_options *options, const char *game_data_path)
{
  log_assert(options);
  log_assert(game_data_path);

  char *path_tmp;

  path_tmp = util_str_merge(game_data_path, "/SCRIPT");
  cnh_redir_add("/SCRIPT", path_tmp);
  free(path_tmp);
}

static void nx2hook_fs_redir_fonts(
    struct nx2hook_options *options, const char *game_data_path)
{
  log_assert(options);
  log_assert(game_data_path);

  char *path_tmp;

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

static void nx2hook_fs_redir_mission(
    struct nx2hook_options *options, const char *game_data_path)
{
  log_assert(options);
  log_assert(game_data_path);

  char *path_tmp;

  /* mission.txt, game expects lower case file */
  path_tmp = util_str_merge(game_data_path, "/mission.txt");
  cnh_redir_add("/mission.txt", path_tmp);
  free(path_tmp);
}

static void nx2hook_fs_redir_usb()
{
  /* Needed on newer kernels */
  cnh_redir_add("/proc/bus/usb/devices", "/sys/kernel/debug/usb/devices");
  patch_usbfs_init();
}

static void nx2hook_fs_redirs_init(
    struct nx2hook_options *options, const char *game_data_path)
{
  log_assert(options);
  log_assert(game_data_path);

  /* Expecting all assets in the piu game (asset) folder. redirects for files
     that are not in there by default for reasons */
  nx2hook_fs_redir_settings(options);
  nx2hook_fs_redir_script(options, game_data_path);
  nx2hook_fs_redir_fonts(options, game_data_path);
  nx2hook_fs_redir_mission(options, game_data_path);

  nx2hook_fs_redir_usb();
}

static void nx2hook_patch_fs_mounting_init()
{
  patch_mounts_init();

  patch_usb_mnt_init();
  patch_usb_unlock_init();
}

static void nx2hook_patch_gfx_init(struct nx2hook_options *options)
{
  log_assert(options);

  patch_gfx_init();

  if (options->patch.gfx.scaling_mode != PATCH_GFX_SCALE_MODE_INVALID) {
    patch_gfx_scale(options->patch.gfx.scaling_mode);
  }
}

static void nx2hook_patch_main_loop_init(struct nx2hook_options *options)
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
}

static void nx2hook_patch_sound_init(struct nx2hook_options *options)
{
  log_assert(options);

  patch_sound_init(options->patch.sound.device);
}

static void nx2hook_patch_sigsegv_init(struct nx2hook_options *options)
{
  log_assert(options);

  patch_sigsegv_init(options->patch.sigsegv.halt_on_segv);
}

static void nx2hook_patch_dongle_init()
{
  patch_microdog40_init(
      (const uint8_t *) _binary_nx2hook_dog_key_start,
      ((uintptr_t) &_binary_nx2hook_dog_key_end -
       (uintptr_t) &_binary_nx2hook_dog_key_start));
}

static void nx2hook_patch_hdd_check_init()
{
  patch_hdd_check_init(
      (const uint8_t *) _binary_nx2hook_hdd_raw_start,
      ((uintptr_t) &_binary_nx2hook_hdd_raw_end -
       (uintptr_t) &_binary_nx2hook_hdd_raw_start));
}

static void nx2hook_patch_piuio_init(struct nx2hook_options *options)
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

    patch_piuio_init(abs_path_iolib);
    free(abs_path_iolib);
  }
}

static void nx2hook_patch_profile()
{
  nx2hook_profile_gen_init();
}

static void nx2hook_patch_pumpnet(struct nx2hook_options *options)
{
  if (options->patch.net.server && options->patch.net.machine_id != 0) {
    patch_net_profile_init(
        ASSET_GAME_VERSION_NX2,
        options->patch.net.server,
        options->patch.net.machine_id,
        options->patch.net.cert_dir_path,
        options->patch.net.verbose_log_output);
  }
}

void nx2hook_constructor(void)
{
  /* Nothing here */
}

void nx2hook_destructor(void)
{
  /* Nothing here */
}

void nx2hook_trap_before_main(int argc, char **argv)
{
  log_assert(argc >= 0);
  log_assert(argv);

  char *game_data_path;
  struct nx2hook_options options;

  nx2hook_bootstrapping(argc, argv, &options, &game_data_path);
  nx2hook_patch_fs_init(&options);
  nx2hook_fs_redirs_init(&options, game_data_path);
  nx2hook_patch_fs_mounting_init();
  nx2hook_patch_gfx_init(&options);
  nx2hook_patch_main_loop_init(&options);
  nx2hook_patch_sound_init(&options);
  nx2hook_patch_sigsegv_init(&options);
  nx2hook_patch_dongle_init();
  nx2hook_patch_hdd_check_init();
  nx2hook_patch_piuio_init(&options);
  // Init order of pumpnet and auto profile generating important: pumpnet hook,
  // if active, needs to be applied before the auto gen profile hook. Otherwise,
  // the game detects that no profile is available and auto generates one even
  // it should take the pumpnet route to grab a profile from a remote server
  nx2hook_patch_pumpnet(&options);
  nx2hook_patch_profile();

  free(game_data_path);

  log_info("Hooking finished");
}

void nx2hook_trap_after_main(void)
{
  patch_net_profile_shutdown();
  patch_piuio_shutdown();
}

LIB_MAIN_CONSTRUCTOR(nx2hook_constructor);
LIB_MAIN_DESTRUCTOR(nx2hook_destructor);
LIB_MAIN_TRAP_MAIN(nx2hook_trap_before_main, nx2hook_trap_after_main);