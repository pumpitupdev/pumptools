#define LOG_MODULE "patch-usb-mnt"

#include "util/fs.h"
#include "util/log.h"

#include "usb-mnt.h"

static const char *patch_usb_mnt_player[2] = {"/mnt/0", "/mnt/1"};
// Invalid files are used to detect if mounting the usb stick was successful
static const char *patch_usb_mnt_player_invalid[2] = {
    "/mnt/0/invalid", "/mnt/1/invalid"};

void patch_usb_mnt_init(void)
{
  for (uint8_t i = 0; i < 2; i++) {
    if (!util_fs_path_exists(patch_usb_mnt_player[i])) {
      log_info("%s does not exist, creating", patch_usb_mnt_player[i]);

      if (!util_fs_mkdir(patch_usb_mnt_player[i])) {
        log_error("Creating directory %s failed", patch_usb_mnt_player[i]);
      }

      if (!util_fs_mkfile(patch_usb_mnt_player_invalid[i])) {
        log_error(
            "Creating invalid indicator file %s failed",
            patch_usb_mnt_player_invalid[i]);
      }
    }
  }

  log_info("Initialized");
}