#include <signal.h>
#include <unistd.h>

#include "hook/patch/microdog40.c"
#include "util/fs.h"
#include "util/log.h"
#include "util/mem.h"

void shutdown(int);

int main(int argc, char *argv[])
{
  if (argc < 2) {
    printf("Usage: %s <keyfile.bin> [debug]\n", argv[0]);
    return 1;
  }

  for (int i = 0; i < argc; i++) {
    if (!strcmp(argv[i], "debug")) {
      util_log_set_level(LOG_LEVEL_DEBUG);
      break;
    }
  }

  size_t key_data_len;
  void *key_data;

  if (!util_file_load(argv[1], &key_data, &key_data_len, false)) {
    log_error("Loading key file %s failed", argv[1]);
    return 2;
  }

  log_info("Running Microdog 4.0 Emulator");
  patch_microdog40_init(key_data, key_data_len);
  util_xfree(&key_data);

  signal(SIGINT, shutdown);

  // Run Daemon
  while (true) {
    sleep(2);

    if (!sec_microdog40d_is_running()) {
      return 0;
    }
  }
}

void shutdown(int sig)
{
  patch_microdog40_shutdown();
}
