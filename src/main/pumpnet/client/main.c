#define LOG_MODULE "pumpnet-client"

#include <stdint.h>
#include <string.h>

#include "asset/nx2/lib/usb-rank.h"
#include "asset/nx2/lib/usb-save.h"

#include "pumpnet/lib/pumpnet.h"

#include "util/fs.h"
#include "util/log.h"
#include "util/mem.h"

static void _print_usage_and_exit(const char *program_name)
{
  log_info(
      "Usage: %s <server addr> <get/put> <game version> <file type> <machine "
      "id> <player ref id> "
      "<in/out file path>",
      program_name);
  exit(EXIT_FAILURE);
}

static size_t _get_size_save_file(enum asset_game_version game_version)
{
  switch (game_version) {
    case ASSET_GAME_VERSION_NX2:
      return ASSET_NX2_USB_SAVE_SIZE;

    default:
      log_die_illegal_state();
      return 0;
  }
}

static size_t _get_size_rank_file(enum asset_game_version game_version)
{
  switch (game_version) {
    case ASSET_GAME_VERSION_NX2:
      return ASSET_NX2_USB_RANK_SIZE;

    default:
      log_die_illegal_state();
      return 0;
  }
}

static size_t _get_size_usb_file(
    enum asset_game_version game_version, enum pumpnet_lib_file_type file_type)
{
  switch (file_type) {
    case PUMPNET_LIB_FILE_TYPE_SAVE:
      return _get_size_save_file(game_version);

    case PUMPNET_LIB_FILE_TYPE_RANK:
      return _get_size_rank_file(game_version);

    default:
      log_die_illegal_state();
      return 0;
  }
}

int main(int argc, char **argv)
{
  util_log_set_level(LOG_LEVEL_DEBUG);

  if (argc < 8) {
    _print_usage_and_exit(argv[0]);
  }

  const char *server_addr = argv[1];
  const char *cmd = argv[2];
  enum asset_game_version game_version = strtol(argv[3], NULL, 10);
  enum pumpnet_lib_file_type file_type = strtol(argv[4], NULL, 10);
  uint64_t machine_id = strtol(argv[5], NULL, 16);
  uint64_t player_ref_id = strtol(argv[6], NULL, 16);
  const char *file_path = argv[7];

  bool success;

  pumpnet_lib_init(game_version, server_addr, machine_id, NULL, true);

  if (!strcmp(cmd, "put")) {
    size_t size;
    void *buffer;

    if (!util_file_load(file_path, &buffer, &size, false)) {
      log_error("Loading data from %s failed", file_path);
      success = false;
    } else {
      success =
          pumpnet_lib_put(file_type, player_ref_id, buffer, sizeof(buffer));
    }
  } else if (!strcmp(cmd, "get")) {
    size_t buffer_size = _get_size_usb_file(game_version, file_type);
    uint8_t *buffer = malloc(buffer_size);

    success = pumpnet_lib_get(file_type, player_ref_id, buffer, sizeof(buffer));

    if (success) {
      if (!util_file_save(file_path, buffer, buffer_size)) {
        log_error("Saving data to %s, size %d failed", file_path, buffer_size);
        success = false;
      }
    }
  } else {
    log_error("Invalid command %s", cmd);
    success = false;
  }

  pumpnet_lib_shutdown();

  if (success) {
    exit(EXIT_SUCCESS);
  } else {
    exit(EXIT_FAILURE);
  }
}