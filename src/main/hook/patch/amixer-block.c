#define LOG_MODULE "patch-amixer-block"

#include "capnhook/hook/lib.h"

#include "util/log.h"
#include "util/str.h"

typedef int (*system_t)(const char *command);
typedef int (*execv_t)(const char *path, char *const argv[]);

static bool patch_axmier_block_initialized;
static system_t patch_amixer_block_real_system;
static execv_t patch_amixer_block_real_execv;

// for fst and fex
int system(const char *command)
{
  if (!patch_amixer_block_real_system) {
    patch_amixer_block_real_system = (system_t) cnh_lib_get_func_addr("system");
  }

  if (patch_axmier_block_initialized) {
    if (util_str_starts_with(command, "amixer")) {
      log_info("Blocking call to amixer: %s", command);
      return 0;
    }
  }

  return patch_amixer_block_real_system(command);
}

// for f2 and prime (yes, they figured using system() isn't a great idea...)
int execv(const char *path, char *const argv[])
{
  if (!patch_amixer_block_real_execv) {
    patch_amixer_block_real_execv = (execv_t) cnh_lib_get_func_addr("execv");
  }

  if (patch_axmier_block_initialized) {
    if (util_str_starts_with(path, "/usr/bin/amixer")) {
      log_info("Blocking call to amixer: %s", path);
      return 0;
    }
  }

  return patch_amixer_block_real_execv(path, argv);
}

void patch_axmier_block_init(void)
{
  patch_axmier_block_initialized = true;
  log_info("Initialized");
}