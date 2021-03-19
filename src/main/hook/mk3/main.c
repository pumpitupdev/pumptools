/**
 * Hook library for MK3 linux ports of Pump It Up.
 */
#define LOG_MODULE "mk3hook"

#include <string.h>

#include "capnhook/hook/lib-main.h"
#include "capnhook/hooklib/redir.h"

#include "hook/core/piu-utils.h"
#include "hook/mk3/1st-2nd-fs-fix.h"
#include "hook/mk3/config.h"
#include "hook/mk3/fmodex.h"
#include "hook/mk3/options.h"
#include "hook/patch/asound-fix.h"
#include "hook/patch/hook-mon.h"
#include "hook/patch/main-loop.h"
#include "hook/patch/piuio-exit.h"
#include "hook/patch/piuio.h"
#include "hook/patch/redir.h"
#include "hook/patch/sigsegv.h"
#include "hook/patch/sound.h"
#include "hook/patch/usb-emu.h"

#include "util/fs.h"
#include "util/glibc.h"
#include "util/log.h"
#include "util/patch.h"
#include "util/proc.h"
#include "util/str.h"
#include "util/sys-info.h"

static void mk3hook_bootstrapping(
    int argc,
    char **argv,
    struct mk3hook_options *options,
    char **game_data_path)
{
  log_assert(argc >= 0);
  log_assert(argv);
  log_assert(options);
  log_assert(game_data_path);

  if (!mk3hook_options_init(argc, argv, options)) {
    exit(0);
  }

  hook_core_piu_log_init(argc, argv);

  hook_core_piu_log_info(argc, argv);

  // Have different arguments to align with newer pump games
  // argv[1]: game folder path
  *game_data_path =
      hook_core_piu_utils_get_and_verify_game_data_path(argc, argv);

  hook_core_piu_utils_verify_root_user();

  log_info("Initializing, piu game data path: %s", *game_data_path);
}

static void mk3hook_patch_config_init(struct mk3hook_options *options)
{
  mk3hook_config_init(options);
}

static void mk3hook_patch_fs_init(struct mk3hook_options *options)
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

static void mk3hook_1st_2nd_fs_redirs(const char *game_data_path_abs)
{
  log_assert(game_data_path_abs);

  // The "datas" directory does not require a redirect as the game uses chdir to
  // move to that folder. For this, the additional patch module "1st-2nd-fs-fix"
  // is required (see below). All files usually local to the piu executable,
  // which are outside of the "datas" folder, have to be redirected.
  cnh_redir_add("INTRO.ANI", util_str_merge(game_data_path_abs, "/INTRO.ANI"));
  cnh_redir_add("intro.ani", util_str_merge(game_data_path_abs, "/INTRO.ANI"));
  cnh_redir_add("spr.res", util_str_merge(game_data_path_abs, "/SPR.RES"));
  cnh_redir_add("SPR.RES", util_str_merge(game_data_path_abs, "/SPR.RES"));
  cnh_redir_add("piu.bin", util_str_merge(game_data_path_abs, "/PIU.BIN"));
  cnh_redir_add("PIU.BIN", util_str_merge(game_data_path_abs, "/PIU.BIN"));
  cnh_redir_add("stage.cfg", util_str_merge(game_data_path_abs, "/STAGE.CFG"));
  cnh_redir_add("STAGE.CFG", util_str_merge(game_data_path_abs, "/STAGE.CFG"));

  // For redirecting additional files for linux ports. Configurable via MK3
  // linux ports config.cfg but causes stack overflow on long paths. Redirecting
  // the paths afterwards is save.
  cnh_redir_add("EFFECTS", util_str_merge(game_data_path_abs, "/EFFECTS/"));
  cnh_redir_add("track", util_str_merge(game_data_path_abs, "/track/"));

  mk3hook_1st_2nd_fs_fix_init(game_data_path_abs);
}

static void mk3hook_3rd_and_up_fs_redirs(const char *game_data_path_abs)
{
  log_assert(game_data_path_abs);

  // Sometimes, the game decides to read stuff using lower case, sometimes upper
  // case *shrug* Also, we don't are about cleaning up the resulting merged
  // strings here. Have some memory leaks!
  cnh_redir_add(".", util_str_merge(game_data_path_abs, "/"));
  cnh_redir_add("audio", util_str_merge(game_data_path_abs, "/AUDIO"));
  cnh_redir_add("AUDIO", util_str_merge(game_data_path_abs, "/AUDIO"));
  cnh_redir_add("bga", util_str_merge(game_data_path_abs, "/BGA"));
  cnh_redir_add("BGA", util_str_merge(game_data_path_abs, "/BGA"));
  cnh_redir_add("step", util_str_merge(game_data_path_abs, "/STEP"));
  cnh_redir_add("STEP", util_str_merge(game_data_path_abs, "/STEP"));
  cnh_redir_add("title", util_str_merge(game_data_path_abs, "/TITLE"));
  cnh_redir_add("TITLE", util_str_merge(game_data_path_abs, "/TITLE"));
  cnh_redir_add("piu.bin", util_str_merge(game_data_path_abs, "/PIU.BIN"));
  cnh_redir_add("PIU.BIN", util_str_merge(game_data_path_abs, "/PIU.BIN"));
  cnh_redir_add("stage.cfg", util_str_merge(game_data_path_abs, "/STAGE.CFG"));
  cnh_redir_add("STAGE.CFG", util_str_merge(game_data_path_abs, "/STAGE.CFG"));
  // Special treatment for extra because keeping the name stage.cfg was not good
  // enough...
  cnh_redir_add("game.cfg", util_str_merge(game_data_path_abs, "/GAME.CFG"));
  cnh_redir_add("GAME.CFG", util_str_merge(game_data_path_abs, "/GAME.CFG"));

  // For redirecting additional files for linux ports. Configurable via MK3
  // linux ports config.cfg but causes stack overflow on long paths. Redirecting
  // the paths afterwards is save.
  cnh_redir_add("EFFECTS", util_str_merge(game_data_path_abs, "/EFFECTS/"));
  cnh_redir_add("track", util_str_merge(game_data_path_abs, "/track/"));
}

static void mk3hook_patch_1st_and_2nd_fs_init(
    struct mk3hook_options *options, const char *game_data_path)
{
  log_assert(options);
  log_assert(game_data_path);

  char *game_data_path_abs = util_fs_get_abs_path(game_data_path);

  if (options->game.fs_1st_2nd) {
    mk3hook_1st_2nd_fs_redirs(game_data_path_abs);
  } else {
    mk3hook_3rd_and_up_fs_redirs(game_data_path_abs);
  }

  free(game_data_path_abs);
}

static void mk3hook_patch_main_loop_init(struct mk3hook_options *options)
{
  log_assert(options);

  patch_main_loop_init(
      true, options->patch.main_loop.disable_built_in_inputs, true);

  if (options->patch.main_loop.x11_input_handler_api_lib) {
    char *abs_path_x11_input_hook_lib = util_fs_get_abs_path(
        options->patch.main_loop.x11_input_handler_api_lib);

    patch_main_loop_add_x11_input_handler(abs_path_x11_input_hook_lib);
    free(abs_path_x11_input_hook_lib);
  }
}

static void mk3hook_patch_sound_init(struct mk3hook_options *options)
{
  log_assert(options);

  patch_asound_fix_init();
  mk3hook_fmodex_init(
      options->patch.sound.debug_output, options->patch.sound.device);
}

static void mk3hook_patch_sigsegv_init(struct mk3hook_options *options)
{
  log_assert(options);

  patch_sigsegv_init(options->patch.sigsegv.halt_on_segv);
}

static void mk3hook_patch_piuio_init(struct mk3hook_options *options)
{
  log_assert(options);

  /* Hook before IO emulation */
  if (options->patch.piuio.exit_test_serv) {
    patch_piuio_exit_init();
  }

  patch_usb_emu_init();

  if (options->patch.piuio.api_lib) {
    char *abs_path_iolib = util_fs_get_abs_path(options->patch.piuio.api_lib);

    patch_piuio_init(abs_path_iolib);
    free(abs_path_iolib);
  }
}

static void mk3hook_fix_argv(int argc, char **argv)
{
  log_assert(argc >= 0);
  log_assert(argv);

  // Game exec main expects the following arguments
  // argv[1]: cfg1 path
  // argv[2]: cfg2 path (optional)
  // Config loading is stubbed, so just provide something

  argv[1] = "./save/config.cfg";

  if (argc > 2) {
    argv[2] = "./save/config.cfg";
  }
}

void mk3hook_constructor(void)
{
  /* Nothing here */
}

void mk3hook_destructor(void)
{
  /* Nothing here */
}

void mk3hook_trap_before_main(int argc, char **argv)
{
  log_assert(argc >= 0);
  log_assert(argv);

  struct mk3hook_options options;
  char *game_data_path;

  mk3hook_bootstrapping(argc, argv, &options, &game_data_path);
  mk3hook_patch_config_init(&options);
  mk3hook_patch_fs_init(&options);
  mk3hook_patch_1st_and_2nd_fs_init(&options, game_data_path);
  mk3hook_patch_main_loop_init(&options);
  mk3hook_patch_sound_init(&options);
  mk3hook_patch_sigsegv_init(&options);
  mk3hook_patch_piuio_init(&options);
  mk3hook_fix_argv(argc, argv);

  free(game_data_path);

  log_info("Hooking finished");
}

void mk3hook_trap_after_main(void)
{
}

LIB_MAIN_CONSTRUCTOR(mk3hook_constructor);
LIB_MAIN_DESTRUCTOR(mk3hook_destructor);
LIB_MAIN_TRAP_MAIN(mk3hook_trap_before_main, mk3hook_trap_after_main);