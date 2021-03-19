#define LOG_MODULE "util-proc"

#include <errno.h>
#include <linux/limits.h>
#include <string.h>
#include <unistd.h>

#include "util/log.h"
#include "util/proc.h"
#include "util/str.h"

static char *
util_sys_read_entries_from_proc_file(const char *path, int *count_out)
{
  const char *sep = "\n";
  char *str = util_str_dup("");
  char *tmp;

  FILE *cmdline = fopen(path, "rb");

  if (!cmdline) {
    log_error("Opening %s failed: %s", path, strerror(errno));
    return str;
  }

  char *arg = 0;
  size_t size = 0;
  int count = 0;

  while (getdelim(&arg, &size, 0, cmdline) != -1) {
    if (strlen(str) > 0) {
      tmp = util_str_merge(str, sep);
      free(str);
      str = tmp;
    }

    tmp = util_str_merge(str, arg);
    free(str);
    str = tmp;
    count++;
  }

  free(arg);
  fclose(cmdline);

  if (count_out) {
    *count_out = count;
  }

  return str;
}

static void util_proc_log_current_working_dir()
{
  char tmp[PATH_MAX];

  if (!getcwd(tmp, sizeof(tmp))) {
    log_error("Getting current working directory failed");
  } else {
    log_info("Current working directory: %s", tmp);
  }
}

static void util_proc_log_current_process()
{
  log_info("PID: %d", getpid());
}

static void util_sys_info_log_path_executable()
{
  char buffer[PATH_MAX];

  if (!util_proc_get_path_executable(buffer, sizeof(buffer))) {
    log_error("Getting path of executable failed");
  } else {
    log_info("Executable: %s", buffer);
  }
}

static void util_sys_info_log_user()
{
  log_info("User: %d", getuid());
}

static void util_sys_info_log_running_as_root()
{
  log_info("Running as root: %d", util_proc_is_running_as_root());
}

static void util_sys_info_log_cmd_line_args()
{
  int count;
  char *str =
      util_sys_read_entries_from_proc_file("/proc/self/cmdline", &count);

  log_info("Cmd arguments (%d):\n%s", count, str);

  free(str);
}

static void util_sys_info_log_env_vars()
{
  int count;
  char *str =
      util_sys_read_entries_from_proc_file("/proc/self/environ", &count);

  log_info("Environment variables (%d):\n%s", count, str);

  free(str);
}

bool util_proc_is_running_as_root()
{
  return getuid() == 0;
}

bool util_proc_get_path_executable(char *buffer, size_t size)
{
  // readlink does not null terminate
  memset(buffer, 0, size);

  return readlink("/proc/self/exe", buffer, size) != -1;
}

bool util_proc_get_folder_path_executable(char *buffer, size_t size)
{
  if (!util_proc_get_path_executable(buffer, size)) {
    return false;
  }

  size_t pos = strlen(buffer) - 1;

  // If executable in the root folder, keep the single /
  while (pos > 0 && buffer[pos] != '/') {
    buffer[pos] = '\0';
    pos--;
  }

  // delete /
  if (pos > 0) {
    buffer[pos] = '\0';
  }

  return true;
}

bool util_proc_get_folder_path_executable_no_ld_linux(char *buffer, size_t size)
{
  if (!util_proc_get_path_executable(buffer, size)) {
    log_error("Getting executable path failed");
    return false;
  }

  if (util_str_ends_with(buffer, "ld-linux.so.2")) {
    int argc;
    char *args =
        util_sys_read_entries_from_proc_file("/proc/self/cmdline", &argc);

    char *tok = strtok(args, "\n");
    bool awaiting_value = false;

    // Skip first token, ld-linux binary
    tok = strtok(NULL, "\n");

    while (tok != NULL) {
      // Check if the next arg is declaring an option key for ld-linux
      if (util_str_starts_with(tok, "--")) {
        awaiting_value = 1;
      } else if (awaiting_value) {
        // Just hit the awaited value for the previously skipped option key
        awaiting_value = false;
      } else {
        // Got executable arg passed to ld-linux
        if (strlen(tok) > size) {
          free(args);
          return false;
        }

        strcpy(buffer, tok);

        // Remove executable name
        // If executable in the root folder, keep the single /
        size_t pos = strlen(buffer) - 1;
        while (pos > 0 && buffer[pos] != '/') {
          buffer[pos] = '\0';
          pos--;
        }

        // delete /
        if (pos > 0) {
          buffer[pos] = '\0';
        }

        free(args);
        return true;
      }

      tok = strtok(NULL, "\n");
    }

    free(args);
    return false;
  } else {
    return util_proc_get_folder_path_executable(buffer, size);
  }
}

void util_proc_log_info()
{
  log_info(
      "================================  Current process information "
      "================================");

  util_proc_log_current_process();
  util_sys_info_log_path_executable();
  util_proc_log_current_working_dir();
  util_sys_info_log_user();
  util_sys_info_log_running_as_root();
  util_sys_info_log_cmd_line_args();
  util_sys_info_log_env_vars();

  log_info(
      "========================================================================"
      "========================");
}