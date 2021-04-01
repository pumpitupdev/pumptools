#include "hook/pro/options.h"

#include "util/options.h"

#define PROHOOK_OPTIONS_STR_GAME_DATA "game.data"
#define PROHOOK_OPTIONS_STR_GAME_SETTINGS "game.settings"
#define PROHOOK_OPTIONS_STR_PATCH_HOOK_MON_FILE "patch.hook_mon.file"
#define PROHOOK_OPTIONS_STR_PATCH_HOOK_MON_FS "patch.hook_mon.fs"
#define PROHOOK_OPTIONS_STR_PATCH_HOOK_MON_IO "patch.hook_mon.io"
#define PROHOOK_OPTIONS_STR_PATCH_HOOK_MON_OPEN "patch.hook_mon.open"
#define PROHOOK_OPTIONS_STR_PATCH_HOOK_MON_USB "patch.hook_mon.usb"
#define PROHOOK_OPTIONS_STR_PATCH_PIUBTN_EMU_LIB "patch.piubtn.emu_lib"
#define PROHOOK_OPTIONS_STR_PATCH_PIUIO_EMU_LIB "patch.piuio.emu_lib"
#define PROHOOK_OPTIONS_STR_PATCH_PIUIO_EXIT_TEST_SERV \
  "patch.piuio_exit.test_serv"
#define PROHOOK_OPTIONS_STR_PATCH_USB_PROFILE_P1_BUS_PORT \
  "patch.usb_profile.p1.bus_port"
#define PROHOOK_OPTIONS_STR_PATCH_USB_PROFILE_P2_BUS_PORT \
  "patch.usb_profile.p2.bus_port"
#define PROHOOK_OPTIONS_STR_PATCH_USB_PROFILE_DEV_NODES \
  "patch.usb_profile.dev_nodes"
#define PROHOOK_OPTIONS_STR_PATCH_X11_EVENT_LOOP_INPUT_HANDLER \
  "patch_x11_event_loop.input_handler_lib"
#define PROHOOK_OPTIONS_STR_PATCH_X11_EVENT_LOOP_INPUT_HANDLER2 \
  "patch_x11_event_loop.input_handler_lib2"
#define PROHOOK_OPTIONS_STR_PATCH_UTIL_LOG_FILE "util.log.file"
#define PROHOOK_OPTIONS_STR_PATCH_UTIL_LOG_LEVEL "util.log.level"

static const struct util_options_def prohook_options_def[] = {
    {
        .name = PROHOOK_OPTIONS_STR_GAME_DATA,
        .description = "Path to game data (game) folder",
        .param = 'd',
        .type = UTIL_OPTIONS_TYPE_STR,
        .is_secret_data = false,
        .default_value.str = "./game",
    },
    {
        .name = PROHOOK_OPTIONS_STR_GAME_SETTINGS,
        .description = "Path to game settings (data) folder",
        .param = 's',
        .type = UTIL_OPTIONS_TYPE_STR,
        .is_secret_data = false,
        .default_value.str = "./save",
    },
    {
        .name = PROHOOK_OPTIONS_STR_PATCH_HOOK_MON_FILE,
        .description = "Enable file call monitoring",
        .param = 'f',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .is_secret_data = false,
        .default_value.b = false,
    },
    {
        .name = PROHOOK_OPTIONS_STR_PATCH_HOOK_MON_FS,
        .description = "Enable file system related call monitoring",
        .param = 'g',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .is_secret_data = false,
        .default_value.b = false,
    },
    {
        .name = PROHOOK_OPTIONS_STR_PATCH_HOOK_MON_IO,
        .description = "Enable IO syscall monitoring",
        .param = 'i',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .is_secret_data = false,
        .default_value.b = false,
    },
    {
        .name = PROHOOK_OPTIONS_STR_PATCH_HOOK_MON_OPEN,
        .description = "Enable any file/IO open call monitoring",
        .param = 'j',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .is_secret_data = false,
        .default_value.b = false,
    },
    {
        .name = PROHOOK_OPTIONS_STR_PATCH_HOOK_MON_USB,
        .description = "Enable libusb call monitoring",
        .param = 'u',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .is_secret_data = false,
        .default_value.b = false,
    },
    {
        .name = PROHOOK_OPTIONS_STR_PATCH_PIUBTN_EMU_LIB,
        .description =
            "Path to library implementing the piubtn api for piubtn emulation",
        .param = 'b',
        .type = UTIL_OPTIONS_TYPE_STR,
        .is_secret_data = false,
        .default_value.str = NULL,
    },
    {
        .name = PROHOOK_OPTIONS_STR_PATCH_PIUIO_EMU_LIB,
        .description =
            "Path to library implementing the piuio api for piuio emulation",
        .param = 'p',
        .type = UTIL_OPTIONS_TYPE_STR,
        .is_secret_data = false,
        .default_value.str = NULL,
    },
    {
        .name = PROHOOK_OPTIONS_STR_PATCH_PIUIO_EXIT_TEST_SERV,
        .description = "Enable game exit on Test + Service",
        .param = 'e',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .is_secret_data = false,
        .default_value.b = false,
    },
    {
        .name = PROHOOK_OPTIONS_STR_PATCH_USB_PROFILE_P1_BUS_PORT,
        .description = "Bus and port (format: X-X, e.g. 1-2) of the USB slot "
                       "to assign to Player 1 when a USB thumb "
                       "drive is plugged in",
        .param = 'a',
        .type = UTIL_OPTIONS_TYPE_STR,
        .is_secret_data = false,
        .default_value.str = NULL,
    },
    {
        .name = PROHOOK_OPTIONS_STR_PATCH_USB_PROFILE_P2_BUS_PORT,
        .description = "Bus and port (format: X-X, e.g. 1-2) of the USB slot "
                       "to assign to Player 2 when a USB thumb "
                       "drive is plugged in",
        .param = 'k',
        .type = UTIL_OPTIONS_TYPE_STR,
        .is_secret_data = false,
        .default_value.str = NULL,
    },
    {
        .name = PROHOOK_OPTIONS_STR_PATCH_USB_PROFILE_DEV_NODES,
        .description = "Device nodes of plugged in USB thumb drives to "
                       "consider for mounting for usb profiles, format: "
                       "sdX,sdY e.g. sde,sdf",
        .param = 't',
        .type = UTIL_OPTIONS_TYPE_STR,
        .is_secret_data = false,
        .default_value.str = NULL,
    },
    {
        .name = PROHOOK_OPTIONS_STR_PATCH_X11_EVENT_LOOP_INPUT_HANDLER,
        .description = "Path to a library implementing the x11-input-handler "
                       "api to capture X11 keyboard inputs",
        .param = 'q',
        .type = UTIL_OPTIONS_TYPE_STR,
        .is_secret_data = false,
        .default_value.str = NULL,
    },
    {
        .name = PROHOOK_OPTIONS_STR_PATCH_X11_EVENT_LOOP_INPUT_HANDLER2,
        .description = "Path to a second library implementing the "
                       "x11-input-handler api to capture X11 keyboard inputs",
        .param = 'r',
        .type = UTIL_OPTIONS_TYPE_STR,
        .is_secret_data = false,
        .default_value.str = NULL,
    },
    {
        .name = PROHOOK_OPTIONS_STR_PATCH_UTIL_LOG_FILE,
        .description = "Print the log output to the specified file",
        .param = 'o',
        .type = UTIL_OPTIONS_TYPE_STR,
        .is_secret_data = false,
        .default_value.str = "pumptools.log",
    },
    {
        .name = PROHOOK_OPTIONS_STR_PATCH_UTIL_LOG_LEVEL,
        .description = "Set the log level (0-4)",
        .param = 'l',
        .type = UTIL_OPTIONS_TYPE_INT,
        .is_secret_data = false,
        .default_value.i = LOG_LEVEL_DEBUG,
    },
};

static const struct util_options_defs prohook_options_defs = {
    .usage_header =
        "Pumptools prohook for Pump It Up: Pro, build " __DATE__ " " __TIME__ ", gitrev " STRINGIFY(GITREV) "\n"
        "Usage: LD_PRELOAD=./prohook.so [game exec] [game data path] <options>",
    .usage_param = 'h',
    .defs = prohook_options_def,
    .ndefs = lengthof(prohook_options_def)
};

bool prohook_options_init(
    int argc, char **argv, struct prohook_options *options)
{
  log_assert(argv);
  log_assert(options);

  struct util_options_opts *options_opt;

  util_options_init(argc, argv);
  options_opt = util_options_get(&prohook_options_defs);

  if (!options_opt) {
    return false;
  }

  options->game.data =
      util_options_get_str(options_opt, PROHOOK_OPTIONS_STR_GAME_DATA);
  options->game.settings =
      util_options_get_str(options_opt, PROHOOK_OPTIONS_STR_GAME_SETTINGS);
  options->patch.hook_mon.file = util_options_get_bool(
      options_opt, PROHOOK_OPTIONS_STR_PATCH_HOOK_MON_FILE);
  options->patch.hook_mon.fs =
      util_options_get_bool(options_opt, PROHOOK_OPTIONS_STR_PATCH_HOOK_MON_FS);
  options->patch.hook_mon.io =
      util_options_get_bool(options_opt, PROHOOK_OPTIONS_STR_PATCH_HOOK_MON_IO);
  options->patch.hook_mon.open = util_options_get_bool(
      options_opt, PROHOOK_OPTIONS_STR_PATCH_HOOK_MON_OPEN);
  options->patch.hook_mon.usb = util_options_get_bool(
      options_opt, PROHOOK_OPTIONS_STR_PATCH_HOOK_MON_USB);
  options->patch.piubtn.api_lib = util_options_get_str(
      options_opt, PROHOOK_OPTIONS_STR_PATCH_PIUBTN_EMU_LIB);
  options->patch.piuio.api_lib = util_options_get_str(
      options_opt, PROHOOK_OPTIONS_STR_PATCH_PIUIO_EMU_LIB);
  options->patch.piuio.exit_test_serv = util_options_get_bool(
      options_opt, PROHOOK_OPTIONS_STR_PATCH_PIUIO_EXIT_TEST_SERV);
  options->patch.usb_profile.device_nodes = util_options_get_str(
      options_opt, PROHOOK_OPTIONS_STR_PATCH_USB_PROFILE_DEV_NODES);
  options->patch.usb_profile.p1_bus_port = util_options_get_str(
      options_opt, PROHOOK_OPTIONS_STR_PATCH_USB_PROFILE_P1_BUS_PORT);
  options->patch.usb_profile.p2_bus_port = util_options_get_str(
      options_opt, PROHOOK_OPTIONS_STR_PATCH_USB_PROFILE_P2_BUS_PORT);
  options->patch.x11_event_loop.api_lib = util_options_get_str(
      options_opt, PROHOOK_OPTIONS_STR_PATCH_X11_EVENT_LOOP_INPUT_HANDLER);
  options->patch.x11_event_loop.api_lib2 = util_options_get_str(
      options_opt, PROHOOK_OPTIONS_STR_PATCH_X11_EVENT_LOOP_INPUT_HANDLER2);
  options->log.file = util_options_get_str(
      options_opt, PROHOOK_OPTIONS_STR_PATCH_UTIL_LOG_FILE);
  options->log.level = (enum util_log_level) util_options_get_int(
      options_opt, PROHOOK_OPTIONS_STR_PATCH_UTIL_LOG_LEVEL);

  return true;
}