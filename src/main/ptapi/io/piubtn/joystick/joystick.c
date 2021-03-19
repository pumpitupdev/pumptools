/**
 * Implementation of the piubtn API to use the game with joysticks/gamepads.
 */
#define LOG_MODULE "ptapi-io-piubtn-joystick"

#include "io/util/joystick.h"
#include "io/util/joystick-util.h"

#include "ptapi/io/piubtn.h"

#include "ptapi/io/piubtn/joystick-util/joystick-conf.h"

#include "util/log.h"
#include "util/proc.h"
#include "util/str.h"

#define MAX_NUM_JOYSTICKS 16
#define CONFIG_FILENAME "/piubtn-joystick-conf.bin"

struct ptapi_io_piubtn_joystick_with_conf {
  struct ptapi_io_piubtn_joystick_util_conf_entry *conf;
  struct io_util_joystick *joystick;
};

static struct ptapi_io_piubtn_joystick_util_conf
    *ptapi_io_piubtn_joystick_util_conf;
static struct ptapi_io_piubtn_joystick_with_conf
    ptapi_io_piubtn_joysticks_with_conf[MAX_NUM_JOYSTICKS];
static size_t ptapi_io_piubtn_num_active_joysticks;

static bool ptapi_io_piubtn_merge_inputs(
    enum PTAPI_IO_PIUBTN_JOYSTICK_UTIL_CONF_INPUT input)
{
  bool pressed;

  pressed = false;

  for (size_t i = 0; i < ptapi_io_piubtn_num_active_joysticks; i++) {
    if (ptapi_io_piubtn_joysticks_with_conf[i].joystick) {
      pressed =
          pressed ||
          io_util_joystick_is_button_pressed(
              ptapi_io_piubtn_joysticks_with_conf[i].joystick,
              ptapi_io_piubtn_joysticks_with_conf[i].conf->button_map[input]);
    }
  }

  return pressed;
}

const char *ptapi_io_piubtn_ident(void)
{
  return "joystick";
}

bool ptapi_io_piubtn_open()
{
  char path[PATH_MAX];
  char *config_path;
  struct io_util_joystick_util_device_info device_info[MAX_NUM_JOYSTICKS];
  size_t devices_connected;

  // The game changes the working directory to the 'game' sub-folder. Therefore,
  // ./my-config does not work here.
  if (!util_proc_get_folder_path_executable_no_ld_linux(path, sizeof(path))) {
    log_error("Getting executable folder path failed.");
    return false;
  }

  config_path = util_str_merge(path, CONFIG_FILENAME);

  if (!ptapi_io_piubtn_joystick_util_conf_read_from_file(
          &ptapi_io_piubtn_joystick_util_conf, config_path)) {
    log_error("Loading joystick config file %s failed.", CONFIG_FILENAME);
    free(config_path);
    return false;
  }

  free(config_path);

  devices_connected =
      io_util_joystick_util_scan(device_info, MAX_NUM_JOYSTICKS);

  log_debug("Found connected joysticks: %d", devices_connected);

  ptapi_io_piubtn_num_active_joysticks = 0;

  memset(
      ptapi_io_piubtn_joysticks_with_conf,
      0,
      sizeof(struct ptapi_io_piubtn_joystick_with_conf) * MAX_NUM_JOYSTICKS);

  for (size_t i = 0; i < devices_connected; i++) {
    log_debug(
        "%d: %s -> %s, buttons %d, axes %d\n",
        i,
        device_info[i].name,
        device_info[i].dev_path,
        device_info[i].num_buttons,
        device_info[i].num_axes);

    // Find a matching configuration
    for (size_t j = 0; j < ptapi_io_piubtn_joystick_util_conf->num_entries;
         j++) {

      if (!strcmp(
              ptapi_io_piubtn_joystick_util_conf->entries[j].name,
              device_info[i].name) &&
          !strcmp(
              ptapi_io_piubtn_joystick_util_conf->entries[j].dev_path,
              device_info[i].dev_path)) {
        log_debug(
            "Found matching configuration for %s -> %s",
            device_info[i].name,
            device_info[i].dev_path);

        ptapi_io_piubtn_joysticks_with_conf
            [ptapi_io_piubtn_num_active_joysticks]
                .conf = &ptapi_io_piubtn_joystick_util_conf->entries[j];
        ptapi_io_piubtn_num_active_joysticks++;
      }
    }
  }

  // Open all joysticks with a matching configuration

  log_debug("Opening %d joysticks...", ptapi_io_piubtn_num_active_joysticks);

  for (size_t i = 0; i < ptapi_io_piubtn_num_active_joysticks; i++) {

    if (!io_util_joystick_open(
            ptapi_io_piubtn_joysticks_with_conf[i].conf->dev_path,
            &ptapi_io_piubtn_joysticks_with_conf[i].joystick)) {
      log_warn(
          "Could not open joystick %s -> %s, ignoring",
          ptapi_io_piubtn_joysticks_with_conf[i].conf->name,
          ptapi_io_piubtn_joysticks_with_conf[i].conf->dev_path);
    } else {
      log_info(
          "Opened joystick %s -> %s",
          ptapi_io_piubtn_joysticks_with_conf[i].conf->name,
          ptapi_io_piubtn_joysticks_with_conf[i].conf->dev_path);
    }
  }

  return true;
}

void ptapi_io_piubtn_close(void)
{
  free(ptapi_io_piubtn_joystick_util_conf);

  for (size_t i = 0; i < ptapi_io_piubtn_num_active_joysticks; i++) {
    if (ptapi_io_piubtn_joysticks_with_conf[i].joystick) {
      io_util_joystick_close(ptapi_io_piubtn_joysticks_with_conf[i].joystick);
    }
  }
}

bool ptapi_io_piubtn_recv(void)
{
  bool success;

  success = true;

  for (size_t i = 0; i < ptapi_io_piubtn_num_active_joysticks; i++) {
    if (ptapi_io_piubtn_joysticks_with_conf[i].joystick) {
      success = io_util_joystick_update(
          ptapi_io_piubtn_joysticks_with_conf[i].joystick);

      if (!success) {
        log_error(
            "Updating joystick %s -> %s failed",
            ptapi_io_piubtn_joysticks_with_conf[i].conf->name,
            ptapi_io_piubtn_joysticks_with_conf[i].conf->dev_path);
      }
    }
  }

  return success;
}

bool ptapi_io_piubtn_send(void)
{
  return true;
}

void ptapi_io_piubtn_get_input(
    uint8_t player, struct ptapi_io_piubtn_inputs *inputs)
{
  if (player == 0) {
    inputs->start = ptapi_io_piubtn_merge_inputs(
        PTAPI_IO_PIUBTN_JOYSTICK_UTIL_CONF_INPUT_P1_START);
    inputs->back = ptapi_io_piubtn_merge_inputs(
        PTAPI_IO_PIUBTN_JOYSTICK_UTIL_CONF_INPUT_P1_BACK);
    inputs->left = ptapi_io_piubtn_merge_inputs(
        PTAPI_IO_PIUBTN_JOYSTICK_UTIL_CONF_INPUT_P1_LEFT);
    inputs->right = ptapi_io_piubtn_merge_inputs(
        PTAPI_IO_PIUBTN_JOYSTICK_UTIL_CONF_INPUT_P1_RIGHT);
  } else {
    inputs->start = ptapi_io_piubtn_merge_inputs(
        PTAPI_IO_PIUBTN_JOYSTICK_UTIL_CONF_INPUT_P2_START);
    inputs->back = ptapi_io_piubtn_merge_inputs(
        PTAPI_IO_PIUBTN_JOYSTICK_UTIL_CONF_INPUT_P2_BACK);
    inputs->left = ptapi_io_piubtn_merge_inputs(
        PTAPI_IO_PIUBTN_JOYSTICK_UTIL_CONF_INPUT_P2_LEFT);
    inputs->right = ptapi_io_piubtn_merge_inputs(
        PTAPI_IO_PIUBTN_JOYSTICK_UTIL_CONF_INPUT_P2_RIGHT);
  }
}

void ptapi_io_piubtn_set_output(
    uint8_t player, const struct ptapi_io_piubtn_outputs *outputs)
{
  /* Not supported */
}