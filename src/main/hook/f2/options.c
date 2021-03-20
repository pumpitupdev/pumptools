#include "hook/f2/options.h"

#include "util/options.h"

#define F2HOOK_OPTIONS_STR_GAME_SETTINGS "game.settings"
#define F2HOOK_OPTIONS_STR_PATCH_GFX_SCALING_MODE "gfx.scaling_mode"
#define F2HOOK_OPTIONS_STR_PATCH_HOOK_MON_FILE "patch.hook_mon.file"
#define F2HOOK_OPTIONS_STR_PATCH_HOOK_MON_FS "patch.hook_mon.fs"
#define F2HOOK_OPTIONS_STR_PATCH_HOOK_MON_IO "patch.hook_mon.io"
#define F2HOOK_OPTIONS_STR_PATCH_HOOK_MON_OPEN "patch.hook_mon.open"
#define F2HOOK_OPTIONS_STR_PATCH_HOOK_MON_USB "patch.hook_mon.usb"
#define F2HOOK_OPTIONS_STR_PATCH_HOOK_MAIN_LOOP_DISABLE_BUILT_IN_INPUTS \
  "patch.hook_main_loop.disable_built_in_inputs"
#define F2HOOK_OPTIONS_STR_PATCH_HOOK_MAIN_LOOP_X11_INPUT_HANDLER \
  "patch_hook_main_loop.x11_input_handler"
#define F2HOOK_OPTIONS_STR_PATCH_NET_PROFILE_SERVER "patch.net_profile.server"
#define F2HOOK_OPTIONS_STR_PATCH_NET_PROFILE_MACHINE_ID \
  "patch.net_profile.machine_id"
#define F2HOOK_OPTIONS_STR_PATCH_PIUIO_EMU_LIB "patch.piuio.emu_lib"
#define F2HOOK_OPTIONS_STR_PATCH_PIUIO_EXIT_TEST_SERV \
  "patch.piuio_exit.test_serv"
#define F2HOOK_OPTIONS_STR_PATCH_SOUND_DEVICE "patch.sound.device"
#define F2HOOK_OPTIONS_STR_PATCH_SIGSEGV_HALT_ON_SEGV \
  "patch.sigsegv.halt_on_segv"
#define F2HOOK_OPTIONS_STR_PATCH_UTIL_LOG_FILE "util.log.file"
#define F2HOOK_OPTIONS_STR_PATCH_UTIL_LOG_LEVEL "util.log.level"

const struct util_options_def f2hook_options_def[] = {
    {
        .name = F2HOOK_OPTIONS_STR_GAME_SETTINGS,
        .description = "Path to game settings (SETTINGS) folder",
        .param = 's',
        .type = UTIL_OPTIONS_TYPE_STR,
        .default_value.str = "./save",
    },
    {
        .name = F2HOOK_OPTIONS_STR_PATCH_GFX_SCALING_MODE,
        .description = "Set a scaling mode for the rendered output. Available modes: 0 = disabled, 1 = SD 480 to pillarbox HD 720, 2 = SD 480 to pillarbox HD 1080, 3 = SD 480 to SD 960, 4 = HD 720 to HD 1080",
        .param = 'z',
        .type = UTIL_OPTIONS_TYPE_INT,
        .default_value.i = 0,
    },
    {
        .name = F2HOOK_OPTIONS_STR_PATCH_HOOK_MON_FILE,
        .description = "Enable file call monitoring",
        .param = 'f',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .default_value.b = false,
    },
    {
        .name = F2HOOK_OPTIONS_STR_PATCH_HOOK_MON_FS,
        .description = "Enable file system related call monitoring",
        .param = 'g',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .default_value.b = false,
    },
    {
        .name = F2HOOK_OPTIONS_STR_PATCH_HOOK_MON_IO,
        .description = "Enable IO syscall monitoring",
        .param = 'i',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .default_value.b = false,
    },
    {
        .name = F2HOOK_OPTIONS_STR_PATCH_HOOK_MON_OPEN,
        .description = "Enable any file/IO open call monitoring",
        .param = 'j',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .default_value.b = false,
    },
    {
        .name = F2HOOK_OPTIONS_STR_PATCH_HOOK_MON_USB,
        .description = "Enable libusb call monitoring",
        .param = 'u',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .default_value.b = false,
    },
    {
        .name = F2HOOK_OPTIONS_STR_PATCH_HOOK_MAIN_LOOP_DISABLE_BUILT_IN_INPUTS,
        .description = "Disable the built in keyboard inputs for test (F1), "
                       "service (F2) and clear (F3)",
        .param = 'k',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .default_value.b = false,
    },
    {
        .name = F2HOOK_OPTIONS_STR_PATCH_HOOK_MAIN_LOOP_X11_INPUT_HANDLER,
        .description = "Path to a library implementing the x11-input-handler "
                       "api to capture X11 keyboard inputs",
        .param = 'q',
        .type = UTIL_OPTIONS_TYPE_STR,
        .default_value.str = NULL,
    },
    {
        .name = F2HOOK_OPTIONS_STR_PATCH_NET_PROFILE_SERVER,
        .description =
            "Address and port of pumpnet server (e.g. 127.0.0.1:1234)",
        .param = 'n',
        .type = UTIL_OPTIONS_TYPE_STR,
        .default_value.str = NULL,
    },
    {
        .name = F2HOOK_OPTIONS_STR_PATCH_NET_PROFILE_MACHINE_ID,
        .description = "Machine id for pumpnet",
        .param = 'm',
        .type = UTIL_OPTIONS_TYPE_STR,
        .default_value.str = "0",
    },
    {
        .name = F2HOOK_OPTIONS_STR_PATCH_PIUIO_EMU_LIB,
        .description =
            "Path to library implementing the piuio api for piuio emulation",
        .param = 'p',
        .type = UTIL_OPTIONS_TYPE_STR,
        .default_value.str = NULL,
    },
    {
        .name = F2HOOK_OPTIONS_STR_PATCH_PIUIO_EXIT_TEST_SERV,
        .description = "Enable game exit on Test + Service",
        .param = 'e',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .default_value.b = false,
    },
    {
        .name = F2HOOK_OPTIONS_STR_PATCH_SOUND_DEVICE,
        .description = "Select the sound device to open on snd_pcm_open",
        .param = 'a',
        .type = UTIL_OPTIONS_TYPE_STR,
        .default_value.str = "dmix",
    },
    {
        .name = F2HOOK_OPTIONS_STR_PATCH_SIGSEGV_HALT_ON_SEGV,
        .description = "Halt on sigsegv to attach a debugger to the process",
        .param = 'd',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .default_value.str = false,
    },
    {
        .name = F2HOOK_OPTIONS_STR_PATCH_UTIL_LOG_FILE,
        .description = "Print the log output to the specified file",
        .param = 'o',
        .type = UTIL_OPTIONS_TYPE_STR,
        .default_value.str = "pumptools.log",
    },
    {
        .name = F2HOOK_OPTIONS_STR_PATCH_UTIL_LOG_LEVEL,
        .description = "Set the log level (0-4)",
        .param = 'l',
        .type = UTIL_OPTIONS_TYPE_INT,
        .default_value.i = LOG_LEVEL_DEBUG,
    },
};

const struct util_options_defs f2hook_options_defs = {
    .usage_header =
        "Pumptools f2hook for Pump It Up: Fiesta 2, build " __DATE__ " " __TIME__ ", gitrev " STRINGIFY(GITREV) "\n"
        "Usage: LD_PRELOAD=./f2hook.so [game exec] [game data path] <options>",
    .usage_param = 'h',
    .defs = f2hook_options_def,
    .ndefs = lengthof(f2hook_options_def)
};

bool f2hook_options_init(int argc, char **argv, struct f2hook_options *options)
{
  log_assert(argv);
  log_assert(options);

  struct util_options_opts *options_opt;

  util_options_init(argc, argv);
  options_opt = util_options_get(&f2hook_options_defs);

  if (!options_opt) {
    return false;
  }

  options->game.settings =
      util_options_get_str(options_opt, F2HOOK_OPTIONS_STR_GAME_SETTINGS);
  options->patch.gfx.scaling_mode = util_options_get_int(
      options_opt, F2HOOK_OPTIONS_STR_PATCH_GFX_SCALING_MODE);
  options->patch.hook_mon.file = util_options_get_bool(
      options_opt, F2HOOK_OPTIONS_STR_PATCH_HOOK_MON_FILE);
  options->patch.hook_mon.fs =
      util_options_get_bool(options_opt, F2HOOK_OPTIONS_STR_PATCH_HOOK_MON_FS);
  options->patch.hook_mon.io =
      util_options_get_bool(options_opt, F2HOOK_OPTIONS_STR_PATCH_HOOK_MON_IO);
  options->patch.hook_mon.open = util_options_get_bool(
      options_opt, F2HOOK_OPTIONS_STR_PATCH_HOOK_MON_OPEN);
  options->patch.hook_mon.usb =
      util_options_get_bool(options_opt, F2HOOK_OPTIONS_STR_PATCH_HOOK_MON_USB);
  options->patch.main_loop.disable_built_in_inputs = util_options_get_bool(
      options_opt,
      F2HOOK_OPTIONS_STR_PATCH_HOOK_MAIN_LOOP_DISABLE_BUILT_IN_INPUTS);
  options->patch.main_loop.x11_input_handler_api_lib = util_options_get_str(
      options_opt, F2HOOK_OPTIONS_STR_PATCH_HOOK_MAIN_LOOP_X11_INPUT_HANDLER);
  options->patch.net.server = util_options_get_str(
      options_opt, F2HOOK_OPTIONS_STR_PATCH_NET_PROFILE_SERVER);
  options->patch.net.machine_id = strtoul(
      util_options_get_str(
          options_opt, F2HOOK_OPTIONS_STR_PATCH_NET_PROFILE_MACHINE_ID),
      NULL,
      16);
  options->patch.piuio.api_lib =
      util_options_get_str(options_opt, F2HOOK_OPTIONS_STR_PATCH_PIUIO_EMU_LIB);
  options->patch.piuio.exit_test_serv = util_options_get_bool(
      options_opt, F2HOOK_OPTIONS_STR_PATCH_PIUIO_EXIT_TEST_SERV);
  options->patch.sound.device =
      util_options_get_str(options_opt, F2HOOK_OPTIONS_STR_PATCH_SOUND_DEVICE);
  options->patch.sigsegv.halt_on_segv = util_options_get_bool(
      options_opt, F2HOOK_OPTIONS_STR_PATCH_SIGSEGV_HALT_ON_SEGV);
  options->log.file =
      util_options_get_str(options_opt, F2HOOK_OPTIONS_STR_PATCH_UTIL_LOG_FILE);
  options->log.level = (enum util_log_level) util_options_get_int(
      options_opt, F2HOOK_OPTIONS_STR_PATCH_UTIL_LOG_LEVEL);

  return true;
}