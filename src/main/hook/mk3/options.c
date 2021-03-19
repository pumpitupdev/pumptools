#include "hook/mk3/options.h"

#include "util/options.h"

#define MK3HOOK_OPTIONS_STR_GAME_SETTINGS "game.settings"
#define MK3HOOK_OPTIONS_STR_GAME_SYNC_OFFSET "game.sync_offset"
#define MK3HOOK_OPTIONS_STR_GAME_SYNC_MULTIPLIER "game.sync_multiplier"
#define MK3HOOK_OPTIONS_STR_GAME_MUSIC_VOL "game.music_vol"
#define MK3HOOK_OPTIONS_STR_GAME_SFX_VOL "game.sfx_vol"
#define MK3HOOK_OPTIONS_STR_GAME_1ST_2ND_FS "game.1st_2nd_fs"
#define MK3HOOK_OPTIONS_STR_PATCH_GFX_WINDOWED "patch.gfx.windowed"
#define MK3HOOK_OPTIONS_STR_PATCH_HOOK_MON_FILE "patch.hook_mon.file"
#define MK3HOOK_OPTIONS_STR_PATCH_HOOK_MON_FS "patch.hook_mon.fs"
#define MK3HOOK_OPTIONS_STR_PATCH_HOOK_MON_IO "patch.hook_mon.io"
#define MK3HOOK_OPTIONS_STR_PATCH_HOOK_MON_OPEN "patch.hook_mon.open"
#define MK3HOOK_OPTIONS_STR_PATCH_HOOK_MON_USB "patch.hook_mon.usb"
#define MK3HOOK_OPTIONS_STR_PATCH_HOOK_MAIN_LOOP_DISABLE_BUILT_IN_INPUTS \
  "patch.hook_main_loop.disable_built_in_inputs"
#define MK3HOOK_OPTIONS_STR_PATCH_HOOK_MAIN_LOOP_X11_INPUT_HANDLER \
  "patch_hook_main_loop.x11_input_handler"
#define MK3HOOK_OPTIONS_STR_PATCH_PIUIO_EMU_LIB "patch.piuio.emu_lib"
#define MK3HOOK_OPTIONS_STR_PATCH_PIUIO_EXIT_TEST_SERV \
  "patch.piuio_exit.test_serv"
#define MK3HOOK_OPTIONS_STR_PATCH_SOUND_DEVICE "patch.sound.device"
#define MK3HOOK_OPTIONS_STR_PATCH_SOUND_DEBUG_OUTPUT "patch.sound.debug_output"
#define MK3HOOK_OPTIONS_STR_PATCH_SIGSEGV_HALT_ON_SEGV \
  "patch.sigsegv.halt_on_segv"
#define MK3HOOK_OPTIONS_STR_PATCH_UTIL_LOG_FILE "util.log.file"
#define MK3HOOK_OPTIONS_STR_PATCH_UTIL_LOG_LEVEL "util.log.level"

static const struct util_options_def mk3hook_options_def[] = {
    {
        .name = MK3HOOK_OPTIONS_STR_GAME_SETTINGS,
        .description = "Path to game settings (SETTINGS) folder",
        .param = 's',
        .type = UTIL_OPTIONS_TYPE_STR,
        .default_value.str = "./save",
    },
    {
        .name = MK3HOOK_OPTIONS_STR_GAME_SYNC_OFFSET,
        .description = "Sync offset in ms",
        .param = 'y',
        .type = UTIL_OPTIONS_TYPE_INT,
        .default_value.i = 115,
    },
    {
        .name = MK3HOOK_OPTIONS_STR_GAME_SYNC_MULTIPLIER,
        .description = "Sync multiplier",
        .param = 'v',
        .type = UTIL_OPTIONS_TYPE_DOUBLE,
        .default_value.d = 4.16666666666666696272613990004,
    },
    {
        .name = MK3HOOK_OPTIONS_STR_GAME_MUSIC_VOL,
        .description = "Volume of music playback",
        .param = 'm',
        .type = UTIL_OPTIONS_TYPE_DOUBLE,
        .default_value.d = 1.0,
    },
    {
        .name = MK3HOOK_OPTIONS_STR_GAME_SFX_VOL,
        .description = "Volume of sound effects playback",
        .param = 'c',
        .type = UTIL_OPTIONS_TYPE_DOUBLE,
        .default_value.d = 1.0,
    },
    {
        .name = MK3HOOK_OPTIONS_STR_GAME_1ST_2ND_FS,
        .description = "Enable this option when running 1st or 2nd. This uses "
                       "different file system redirects for the "
                       "game directory as well as enables a fix. Without this, "
                       "the game throws errors about not finding files",
        .param = 'x',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .default_value.b = false,
    },
    {
        .name = MK3HOOK_OPTIONS_STR_PATCH_GFX_WINDOWED,
        .description = "Force game into window mode",
        .param = 'w',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .default_value.b = false,
    },
    {
        .name = MK3HOOK_OPTIONS_STR_PATCH_HOOK_MON_FILE,
        .description = "Enable file call monitoring",
        .param = 'f',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .default_value.b = false,
    },
    {
        .name = MK3HOOK_OPTIONS_STR_PATCH_HOOK_MON_FS,
        .description = "Enable file system related call monitoring",
        .param = 'g',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .default_value.b = false,
    },
    {
        .name = MK3HOOK_OPTIONS_STR_PATCH_HOOK_MON_IO,
        .description = "Enable IO syscall monitoring",
        .param = 'i',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .default_value.b = false,
    },
    {
        .name = MK3HOOK_OPTIONS_STR_PATCH_HOOK_MON_OPEN,
        .description = "Enable any file/IO open call monitoring",
        .param = 'j',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .default_value.b = false,
    },
    {
        .name = MK3HOOK_OPTIONS_STR_PATCH_HOOK_MON_USB,
        .description = "Enable libusb call monitoring",
        .param = 'u',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .default_value.b = false,
    },
    {
        .name =
            MK3HOOK_OPTIONS_STR_PATCH_HOOK_MAIN_LOOP_DISABLE_BUILT_IN_INPUTS,
        .description = "Disable the built in keyboard inputs for test (F1), "
                       "service (F2) and clear (F3)",
        .param = 'k',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .default_value.b = false,
    },
    {
        .name = MK3HOOK_OPTIONS_STR_PATCH_HOOK_MAIN_LOOP_X11_INPUT_HANDLER,
        .description = "Path to a library implementing the x11-input-handler "
                       "api to capture X11 keyboard inputs",
        .param = 'q',
        .type = UTIL_OPTIONS_TYPE_STR,
        .default_value.str = NULL,
    },
    {
        .name = MK3HOOK_OPTIONS_STR_PATCH_PIUIO_EMU_LIB,
        .description =
            "Path to library implementing the piuio api for piuio emulation",
        .param = 'p',
        .type = UTIL_OPTIONS_TYPE_STR,
        .default_value.str = NULL,
    },
    {
        .name = MK3HOOK_OPTIONS_STR_PATCH_PIUIO_EXIT_TEST_SERV,
        .description = "Enable game exit on Test + Service",
        .param = 'e',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .default_value.b = false,
    },
    {
        .name = MK3HOOK_OPTIONS_STR_PATCH_SOUND_DEVICE,
        .description = "Select the sound device to open on snd_pcm_open",
        .param = 'a',
        .type = UTIL_OPTIONS_TYPE_STR,
        .default_value.str = NULL,
    },
    {
        .name = MK3HOOK_OPTIONS_STR_PATCH_SOUND_DEBUG_OUTPUT,
        .description = "Enable debug output for the fmod library. This "
                       "requires you to use the fmodexL.so instead of "
                       "the standard fmodex.so",
        .param = 'r',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .default_value.b = false,
    },
    {
        .name = MK3HOOK_OPTIONS_STR_PATCH_SIGSEGV_HALT_ON_SEGV,
        .description = "Halt on sigsegv to attach a debugger to the process",
        .param = 'd',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .default_value.str = false,
    },
    {
        .name = MK3HOOK_OPTIONS_STR_PATCH_UTIL_LOG_FILE,
        .description = "Print the log output to the specified file",
        .param = 'o',
        .type = UTIL_OPTIONS_TYPE_STR,
        .default_value.str = "pumptools.log",
    },
    {
        .name = MK3HOOK_OPTIONS_STR_PATCH_UTIL_LOG_LEVEL,
        .description = "Set the log level (0-4)",
        .param = 'l',
        .type = UTIL_OPTIONS_TYPE_INT,
        .default_value.i = LOG_LEVEL_DEBUG,
    },
};

static const struct util_options_defs mk3hook_options_defs = {
    .usage_header =
        "Pumptools mk3hook for Pump It Up MK3 Linux ports, build " __DATE__ " " __TIME__ ", gitrev " STRINGIFY(GITREV) "\n"
        "Usage: LD_PRELOAD=./mk3hook.so [game exec] [game data path] <options>",
    .usage_param = 'h',
    .defs = mk3hook_options_def,
    .ndefs = lengthof(mk3hook_options_def)
};

bool mk3hook_options_init(
    int argc, char **argv, struct mk3hook_options *options)
{
  log_assert(argv);
  log_assert(options);

  struct util_options_opts *options_opt;

  util_options_init(argc, argv);
  options_opt = util_options_get(&mk3hook_options_defs);

  if (!options_opt) {
    return false;
  }

  options->game.settings =
      util_options_get_str(options_opt, MK3HOOK_OPTIONS_STR_GAME_SETTINGS);
  options->game.sync_offset =
      util_options_get_int(options_opt, MK3HOOK_OPTIONS_STR_GAME_SYNC_OFFSET);
  options->game.sync_multiplier = util_options_get_double(
      options_opt, MK3HOOK_OPTIONS_STR_GAME_SYNC_MULTIPLIER);
  options->game.music_vol =
      util_options_get_double(options_opt, MK3HOOK_OPTIONS_STR_GAME_MUSIC_VOL);
  options->game.sfx_vol =
      util_options_get_double(options_opt, MK3HOOK_OPTIONS_STR_GAME_SFX_VOL);
  options->game.fs_1st_2nd =
      util_options_get_bool(options_opt, MK3HOOK_OPTIONS_STR_GAME_1ST_2ND_FS);
  options->patch.gfx.windowed = util_options_get_bool(
      options_opt, MK3HOOK_OPTIONS_STR_PATCH_GFX_WINDOWED);
  options->patch.hook_mon.file = util_options_get_bool(
      options_opt, MK3HOOK_OPTIONS_STR_PATCH_HOOK_MON_FILE);
  options->patch.hook_mon.fs =
      util_options_get_bool(options_opt, MK3HOOK_OPTIONS_STR_PATCH_HOOK_MON_FS);
  options->patch.hook_mon.io =
      util_options_get_bool(options_opt, MK3HOOK_OPTIONS_STR_PATCH_HOOK_MON_IO);
  options->patch.hook_mon.open = util_options_get_bool(
      options_opt, MK3HOOK_OPTIONS_STR_PATCH_HOOK_MON_OPEN);
  options->patch.hook_mon.usb = util_options_get_bool(
      options_opt, MK3HOOK_OPTIONS_STR_PATCH_HOOK_MON_USB);
  options->patch.main_loop.disable_built_in_inputs = util_options_get_bool(
      options_opt,
      MK3HOOK_OPTIONS_STR_PATCH_HOOK_MAIN_LOOP_DISABLE_BUILT_IN_INPUTS);
  options->patch.main_loop.x11_input_handler_api_lib = util_options_get_str(
      options_opt, MK3HOOK_OPTIONS_STR_PATCH_HOOK_MAIN_LOOP_X11_INPUT_HANDLER);
  options->patch.piuio.api_lib = util_options_get_str(
      options_opt, MK3HOOK_OPTIONS_STR_PATCH_PIUIO_EMU_LIB);
  options->patch.piuio.exit_test_serv = util_options_get_bool(
      options_opt, MK3HOOK_OPTIONS_STR_PATCH_PIUIO_EXIT_TEST_SERV);
  options->patch.sound.device =
      util_options_get_str(options_opt, MK3HOOK_OPTIONS_STR_PATCH_SOUND_DEVICE);
  options->patch.sound.debug_output = util_options_get_bool(
      options_opt, MK3HOOK_OPTIONS_STR_PATCH_SOUND_DEBUG_OUTPUT);
  options->patch.sigsegv.halt_on_segv = util_options_get_bool(
      options_opt, MK3HOOK_OPTIONS_STR_PATCH_SIGSEGV_HALT_ON_SEGV);
  options->log.file = util_options_get_str(
      options_opt, MK3HOOK_OPTIONS_STR_PATCH_UTIL_LOG_FILE);
  options->log.level = (enum util_log_level) util_options_get_int(
      options_opt, MK3HOOK_OPTIONS_STR_PATCH_UTIL_LOG_LEVEL);

  return true;
}