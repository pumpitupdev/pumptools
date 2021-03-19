#define LOG_MODULE "mk3hook-config"

#include <libconfig.h>

#include "hook/mk3/options.h"

#include "util/fs.h"
#include "util/log.h"
#include "util/str.h"

static int mk3hook_config_sync_offset;
static double mk3hook_config_sync_multiplier;
static double mk3hook_config_music_volume;
static double mk3hook_config_sfx_volume;
static int mk3hook_config_fullscreen;
static int mk3hook_config_allow_exit;
static const char *mk3hook_config_save_file;

int config_read_file(config_t *config, const char *filename)
{
  // Just stub file loading
  log_debug("config_read_file: %s", filename);

  return 1;
}

int config_lookup_int(const config_t *config, const char *path, int *value)
{
  log_debug("config_lookup_int: %s", path);

  if (!strcmp(path, "sync_offset")) {
    *value = mk3hook_config_sync_offset;
  } else {
    log_error("Unsupported int entry read: %s", path);
  }

  return 1;
}

int config_lookup_float(const config_t *config, const char *path, double *value)
{
  log_debug("config_lookup_float: %s", path);

  if (!strcmp(path, "sync_multiplier")) {
    *value = mk3hook_config_sync_multiplier;
  } else if (!strcmp(path, "music_volume")) {
    *value = mk3hook_config_music_volume;
  } else if (!strcmp(path, "sfx_volume")) {
    *value = mk3hook_config_sfx_volume;
  } else {
    log_error("Unsupported float entry read: %s", path);
  }

  return 1;
}

int config_lookup_bool(const config_t *config, const char *path, int *value)
{
  log_debug("config_lookup_bool: %s", path);

  if (!strcmp(path, "fullscreen")) {
    *value = mk3hook_config_fullscreen;
  } else if (!strcmp(path, "allow_exit")) {
    *value = mk3hook_config_allow_exit;
  } else {
    log_error("Unsupported bool entry read: %s", path);
  }

  return 1;
}

int config_lookup_string(
    const config_t *config, const char *path, const char **value)
{
  log_debug("config_lookup_string: %s", path);

  if (!strcmp(path, "save_file")) {
    // Note: This is fine as the size of the string pointed to by the returned
    // address is not relevant for any operations using them, e.g. there is no
    // merging/copying with limited stack'd buffer sizes involved like below.
    *value = mk3hook_config_save_file;
  } else if (!strcmp(path, "effect_path")) {
    // Needs to be hardcoded because the stack buffer for this is only 128 bytes
    // large. This is likely fine for most paths and even when resolving
    // relative paths to absolute ones. Still, this is a risk for a buffer
    // overflow. Utilize the redirect module in the main lib hook function to
    // redirect this to something like a local game dir and resolve this to an
    // absolute path
    *value = "EFFECTS/";
  } else if (!strcmp(path, "track_path")) {
    // Needs to be hardcoded because the stack buffer for this is only 32 bytes
    // large. When combining it with the "Track%02i.mp3" part, the buffer will
    // overflow when this path is longer or even resolved to an absolute path.
    // Utilize the redirect module in the main lib hook function to redirect
    // this to something like a local game dir and resolve this to an absolute
    // path
    *value = "track/";
  } else {
    log_error("Unsupported string entry read: %s", path);
  }

  return 1;
}

static void
mk3hook_config_assign_save_file(const struct mk3hook_options *options)
{
  const char *folder = options->game.settings;

  if (!util_fs_path_exists(folder)) {
    log_info("Settings path %s does not exist, creating", folder);

    if (!util_fs_mkdir(folder)) {
      log_error("Creating directory %s failed", folder);
    }
  }

  char *folder_abs = util_fs_get_abs_path(folder);

  if (!folder_abs) {
    log_error("Failed to resolve absolute path for settings folder");
    return;
  }

  // Don't free this one after assigning the result value as this will still be
  // read from. Keep memory leak.
  mk3hook_config_save_file = util_str_merge(folder_abs, "/EEPROM.BIN");
  free(folder_abs);
}

void mk3hook_config_init(const struct mk3hook_options *options)
{
  mk3hook_config_sync_offset = options->game.sync_offset;
  mk3hook_config_sync_multiplier = options->game.sync_multiplier;
  mk3hook_config_music_volume = options->game.music_vol;
  mk3hook_config_sfx_volume = options->game.sfx_vol;
  mk3hook_config_fullscreen = !options->patch.gfx.windowed;
  mk3hook_config_allow_exit = options->patch.piuio.exit_test_serv;

  mk3hook_config_assign_save_file(options);

  log_info("Initialized");
}