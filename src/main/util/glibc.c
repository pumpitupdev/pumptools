#define LOG_MODULE "util-glibc"

#include <gnu/libc-version.h>

#include "util/log.h"

void util_glibc_info_log()
{
  log_info(
      "glibc release '%s', version '%s'",
      gnu_get_libc_release(),
      gnu_get_libc_version());
}