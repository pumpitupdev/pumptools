#include "hook/pri/options.h"

#include "util/options.h"

#define PRIHOOK_OPTIONS_STR_GAME_SETTINGS "game.settings"
#define PRIHOOK_OPTIONS_STR_PATCH_GFX_SCALING_MODE "patch.gfx.scaling_mode"
#define PRIHOOK_OPTIONS_STR_PATCH_HOOK_MON_FILE "patch.hook_mon.file"
#define PRIHOOK_OPTIONS_STR_PATCH_HOOK_MON_FS "patch.hook_mon.fs"
#define PRIHOOK_OPTIONS_STR_PATCH_HOOK_MON_IO "patch.hook_mon.io"
#define PRIHOOK_OPTIONS_STR_PATCH_HOOK_MON_OPEN "patch.hook_mon.open"
#define PRIHOOK_OPTIONS_STR_PATCH_HOOK_MON_USB "patch.hook_mon.usb"
#define PRIHOOK_OPTIONS_STR_PATCH_HOOK_MAIN_LOOP_DISABLE_BUILT_IN_INPUTS \
  "patch.hook_main_loop.disable_built_in_inputs"
#define PRIHOOK_OPTIONS_STR_PATCH_HOOK_MAIN_LOOP_X11_INPUT_HANDLER \
  "patch_hook_main_loop.x11_input_handler"
#define PRIHOOK_OPTIONS_STR_PATCH_NET_SERVER "patch.net.server"
#define PRIHOOK_OPTIONS_STR_PATCH_PIUIO_EMU_LIB "patch.piuio.emu_lib"
#define PRIHOOK_OPTIONS_STR_PATCH_PIUIO_EXIT_TEST_SERV \
  "patch.piuio_exit.test_serv"
#define PRIHOOK_OPTIONS_STR_PATCH_SOUND_DEVICE "patch.sound.device"
#define PRIHOOK_OPTIONS_STR_PATCH_SIGSEGV_HALT_ON_SEGV \
  "patch.sigsegv.halt_on_segv"
#define PRIHOOK_OPTIONS_STR_PATCH_UTIL_LOG_FILE "util.log.file"
#define PRIHOOK_OPTIONS_STR_PATCH_UTIL_LOG_LEVEL "util.log.level"

const struct util_options_def prihook_options_def[] = {
    {
        .name = PRIHOOK_OPTIONS_STR_GAME_SETTINGS,
        .description = "Path to game settings (SETTINGS) folder",
        .param = 's',
        .type = UTIL_OPTIONS_TYPE_STR,
        .is_secret_data = false,
        .default_value.str = "./save",
    },
    {
        .name = PRIHOOK_OPTIONS_STR_PATCH_GFX_SCALING_MODE,
        .description = "Set a scaling mode for the rendered output. Available modes: 0 = disabled, 1 = SD 480 to pillarbox HD 720, 2 = SD 480 to pillarbox HD 1080, 3 = SD 480 to SD 960, 4 = HD 720 to HD 1080",
        .param = 'z',
        .type = UTIL_OPTIONS_TYPE_INT,
        .is_secret_data = false,
        .default_value.i = 0,
    },
    {
        .name = PRIHOOK_OPTIONS_STR_PATCH_HOOK_MON_FILE,
        .description = "Enable file call monitoring",
        .param = 'f',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .is_secret_data = false,
        .default_value.b = false,
    },
    {
        .name = PRIHOOK_OPTIONS_STR_PATCH_HOOK_MON_FS,
        .description = "Enable file system related call monitoring",
        .param = 'g',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .is_secret_data = false,
        .default_value.b = false,
    },
    {
        .name = PRIHOOK_OPTIONS_STR_PATCH_HOOK_MON_IO,
        .description = "Enable IO syscall monitoring",
        .param = 'i',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .is_secret_data = false,
        .default_value.b = false,
    },
    {
        .name = PRIHOOK_OPTIONS_STR_PATCH_HOOK_MON_OPEN,
        .description = "Enable any file/IO open call monitoring",
        .param = 'j',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .is_secret_data = false,
        .default_value.b = false,
    },
    {
        .name = PRIHOOK_OPTIONS_STR_PATCH_HOOK_MON_USB,
        .description = "Enable libusb call monitoring",
        .param = 'u',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .is_secret_data = false,
        .default_value.b = false,
    },
    {
        .name =
            PRIHOOK_OPTIONS_STR_PATCH_HOOK_MAIN_LOOP_DISABLE_BUILT_IN_INPUTS,
        .description = "Disable the built in keyboard inputs for test (F1), "
                       "service (PRI) and clear (F3)",
        .param = 'k',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .is_secret_data = false,
        .default_value.b = false,
    },
    {
        .name = PRIHOOK_OPTIONS_STR_PATCH_HOOK_MAIN_LOOP_X11_INPUT_HANDLER,
        .description = "Path to a library implementing the x11-input-handler "
                       "api to capture X11 keyboard inputs",
        .param = 'q',
        .type = UTIL_OPTIONS_TYPE_STR,
        .is_secret_data = false,
        .default_value.str = NULL,
    },
    {
        .name = PRIHOOK_OPTIONS_STR_PATCH_NET_SERVER,
        .description =
            "Address and port of pumpnet server (e.g. 127.0.0.1:1234)",
        .param = 'n',
        .type = UTIL_OPTIONS_TYPE_STR,
        .is_secret_data = false,
        .default_value.str = NULL,
    },
    {
        .name = PRIHOOK_OPTIONS_STR_PATCH_PIUIO_EMU_LIB,
        .description =
            "Path to library implementing the piuio api for piuio emulation",
        .param = 'p',
        .type = UTIL_OPTIONS_TYPE_STR,
        .is_secret_data = false,
        .default_value.str = NULL,
    },
    {
        .name = PRIHOOK_OPTIONS_STR_PATCH_PIUIO_EXIT_TEST_SERV,
        .description = "Enable game exit on Test + Service",
        .param = 'e',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .is_secret_data = false,
        .default_value.b = false,
    },
    {
        .name = PRIHOOK_OPTIONS_STR_PATCH_SOUND_DEVICE,
        .description = "Select the sound device to open on snd_pcm_open",
        .param = 'a',
        .type = UTIL_OPTIONS_TYPE_STR,
        .is_secret_data = false,
        .default_value.str = "dmix",
    },
    {
        .name = PRIHOOK_OPTIONS_STR_PATCH_SIGSEGV_HALT_ON_SEGV,
        .description = "Halt on sigsegv to attach a debugger to the process",
        .param = 'd',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .is_secret_data = false,
        .default_value.str = false,
    },
    {
        .name = PRIHOOK_OPTIONS_STR_PATCH_UTIL_LOG_FILE,
        .description = "Print the log output to the specified file",
        .param = 'o',
        .type = UTIL_OPTIONS_TYPE_STR,
        .is_secret_data = false,
        .default_value.str = "pumptools.log",
    },
    {
        .name = PRIHOOK_OPTIONS_STR_PATCH_UTIL_LOG_LEVEL,
        .description = "Set the log level (0-4)",
        .param = 'l',
        .type = UTIL_OPTIONS_TYPE_INT,
        .is_secret_data = false,
        .default_value.i = LOG_LEVEL_DEBUG,
    },
};

const struct util_options_defs prihook_options_defs = {
    .usage_header =
        "Pumptools prihook for Pump It Up: Prime, build " __DATE__ " " __TIME__ ", gitrev " STRINGIFY(GITREV) "\n"
        "Usage: LD_PRELOAD=./prihook.so [game exec] [game data path] <options>",
    .usage_param = 'h',
    .defs = prihook_options_def,
    .ndefs = lengthof(prihook_options_def)
};

bool prihook_options_init(
    int argc, char **argv, struct prihook_options *options)
{
  log_assert(argv);
  log_assert(options);

  struct util_options_opts *options_opt;

  util_options_init(argc, argv);
  options_opt = util_options_get(&prihook_options_defs);

  if (!options_opt) {
    return false;
  }

  options->game.settings =
      util_options_get_str(options_opt, PRIHOOK_OPTIONS_STR_GAME_SETTINGS);
  options->patch.gfx.scaling_mode = util_options_get_int(
      options_opt, PRIHOOK_OPTIONS_STR_PATCH_GFX_SCALING_MODE);
  options->patch.hook_mon.file = util_options_get_bool(
      options_opt, PRIHOOK_OPTIONS_STR_PATCH_HOOK_MON_FILE);
  options->patch.hook_mon.fs =
      util_options_get_bool(options_opt, PRIHOOK_OPTIONS_STR_PATCH_HOOK_MON_FS);
  options->patch.hook_mon.io =
      util_options_get_bool(options_opt, PRIHOOK_OPTIONS_STR_PATCH_HOOK_MON_IO);
  options->patch.hook_mon.open = util_options_get_bool(
      options_opt, PRIHOOK_OPTIONS_STR_PATCH_HOOK_MON_OPEN);
  options->patch.hook_mon.usb = util_options_get_bool(
      options_opt, PRIHOOK_OPTIONS_STR_PATCH_HOOK_MON_USB);
  options->patch.main_loop.disable_built_in_inputs = util_options_get_bool(
      options_opt,
      PRIHOOK_OPTIONS_STR_PATCH_HOOK_MAIN_LOOP_DISABLE_BUILT_IN_INPUTS);
  options->patch.main_loop.x11_input_handler_api_lib = util_options_get_str(
      options_opt, PRIHOOK_OPTIONS_STR_PATCH_HOOK_MAIN_LOOP_X11_INPUT_HANDLER);
  options->patch.net.server =
      util_options_get_str(options_opt, PRIHOOK_OPTIONS_STR_PATCH_NET_SERVER);
  options->patch.piuio.api_lib = util_options_get_str(
      options_opt, PRIHOOK_OPTIONS_STR_PATCH_PIUIO_EMU_LIB);
  options->patch.piuio.exit_test_serv = util_options_get_bool(
      options_opt, PRIHOOK_OPTIONS_STR_PATCH_PIUIO_EXIT_TEST_SERV);
  options->patch.sound.device =
      util_options_get_str(options_opt, PRIHOOK_OPTIONS_STR_PATCH_SOUND_DEVICE);
  options->patch.sigsegv.halt_on_segv = util_options_get_bool(
      options_opt, PRIHOOK_OPTIONS_STR_PATCH_SIGSEGV_HALT_ON_SEGV);
  options->log.file = util_options_get_str(
      options_opt, PRIHOOK_OPTIONS_STR_PATCH_UTIL_LOG_FILE);
  options->log.level = (enum util_log_level) util_options_get_int(
      options_opt, PRIHOOK_OPTIONS_STR_PATCH_UTIL_LOG_LEVEL);

  return true;
}