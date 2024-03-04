#define LOG_MODULE "patch-sysinfo"

#include <sys/sysinfo.h>
#include <stdbool.h>

#include "capnhook/hook/lib.h"

#include "util/log.h"

typedef int (*sysinfo_t)(struct sysinfo *info);

static bool patch_sysinfo_initialized;
static sysinfo_t patch_sysinfo_real_sysinfo;
int sysinfo(struct sysinfo *info) {
    if (!patch_sysinfo_real_sysinfo) {
        patch_sysinfo_real_sysinfo = (sysinfo_t)cnh_lib_get_func_addr("sysinfo");
    }

    // Prime will not run in HD if totalram / (mem_unit * 1024) is less than 999999
    int ret = patch_sysinfo_real_sysinfo(info);
    info->totalram = info->totalram * info->mem_unit;
    info->mem_unit = 1;

    return ret;
}

void patch_sysinfo_init()
{
  patch_sysinfo_initialized = true;
  log_info("Initialized");
}
