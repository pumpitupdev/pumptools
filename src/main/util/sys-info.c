#define LOG_MODULE "util-sys-info"

#include <sys/utsname.h>

#include "util/log.h"

static void util_sys_info_log_kernel()
{
  struct utsname buf;

  if (uname(&buf)) {
    log_error("Reading kernel version failed");
    return;
  }

  log_info(
      "%s|%s|%s|%s|%s",
      buf.sysname,
      buf.nodename,
      buf.release,
      buf.version,
      buf.machine);
}

void util_sys_info_log()
{
  util_sys_info_log_kernel();
}
