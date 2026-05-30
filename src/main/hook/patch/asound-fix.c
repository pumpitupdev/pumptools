#define LOG_MODULE "patch-asound-fix"

#include <stdbool.h>
#include <errno.h>
#include <grp.h>
#include <limits.h>
#include <string.h>
#include <sys/types.h>

#include "capnhook/hook/lib.h"

#include "util/log.h"
#include "util/mem.h"
#include "util/str.h"

typedef int (*getgrnam_r_t)(
    const char *name,
    struct group *grp,
    char *buf,
    size_t buflen,
    struct group **result);

static getgrnam_r_t patch_asound_fix_real_getgrnam_r;

static char **patch_asound_fix_split_user_list_str(const char *user_list_str);

static char *patch_asound_fix_dup_span(const char *start, size_t len)
{
  char *out = util_xmalloc(len + 1);

  memcpy(out, start, len);
  out[len] = '\0';

  return out;
}

static bool patch_asound_fix_parse_group_line(
    const char *line, struct group *grp)
{
  const char *c1 = strchr(line, ':');
  const char *c2;
  const char *c3;
  const char *extra;
  char *gid_end;
  long gid;

  if (c1 == NULL) {
    return false;
  }

  c2 = strchr(c1 + 1, ':');

  if (c2 == NULL) {
    return false;
  }

  c3 = strchr(c2 + 1, ':');

  if (c3 == NULL) {
    return false;
  }

  extra = strchr(c3 + 1, ':');

  if (extra != NULL) {
    return false;
  }

  gid = strtol(c2 + 1, &gid_end, 10);

  if (gid_end != c3 || gid < 0 || gid > UINT_MAX) {
    return false;
  }

  grp->gr_name = patch_asound_fix_dup_span(line, c1 - line);
  grp->gr_passwd = patch_asound_fix_dup_span(c1 + 1, c2 - (c1 + 1));
  grp->gr_gid = (gid_t) gid;
  grp->gr_mem = patch_asound_fix_split_user_list_str(c3 + 1);

  return true;
}

static char **patch_asound_fix_split_user_list_str(const char *user_list_str)
{
  char *user_list_str_cpy = util_str_dup(user_list_str);
  char **user_list = util_xmalloc(sizeof(char *));
  size_t len_list = 1;

  char *ptr = strtok(user_list_str_cpy, ",");

  while (ptr != NULL) {
    // last token will likely read beyond newline to next : token for next group
    // entry
    char *new_line_char = strstr(ptr, "\n");

    if (new_line_char) {
      *new_line_char = '\0';
    }

    len_list++;
    user_list = util_xrealloc(user_list, len_list * sizeof(char *));

    user_list[len_list - 2] = util_str_dup(ptr);
    user_list[len_list - 1] = NULL;

    ptr = strtok(NULL, ",");
  }

  free(user_list_str_cpy);

  return user_list;
}

static char *patch_asound_fix_user_list_to_str(char **user_list)
{
  char **ptr = user_list;
  char *str = util_str_dup("\0");

  while (*ptr != NULL) {
    char *mrg = util_str_merge(str, *ptr);
    free(str);
    str = util_str_merge(mrg, ",");
    free(mrg);

    ptr++;
  }

  return str;
}

int getgrnam_r(
    const char *name,
    struct group *grp,
    char *buf,
    size_t buflen,
    struct group **result)
{
  if (!patch_asound_fix_real_getgrnam_r) {
    patch_asound_fix_real_getgrnam_r =
        (getgrnam_r_t) cnh_lib_get_func_addr("getgrnam_r");
  }

  // We expect to use the standard "audio" group here, otherwise detection of
  // this is not possible. Intercept and execute the following re-implementation
  // of the reading logic from file. I don't know if getgrnam_r usually reads
  // from a cached version of /etc/group but I suppose that doesn't matter here.
  if (!strcmp(name, "audio")) {
    log_debug("Trapping getgrnam_r for audio group of libasound");

    FILE *file = fopen("/etc/group", "r");

    if (file == NULL) {
      log_error("Opening /etc/group failed: %s", strerror(errno));
      *result = NULL;
      return -1;
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = (char *) util_xmalloc(file_size + 1);

    if (fread(buffer, file_size, 1, file) != 1) {
      log_error("Reading /etc/group file failed: %s", strerror(errno));
      fclose(file);
      free(buffer);
      *result = NULL;
      return -1;
    }

    fclose(file);
    buffer[file_size] = '\0';

    char *line = buffer;
    char *audio_line = NULL;

    while (line != NULL && *line != '\0') {
      char *next = strchr(line, '\n');

      if (next != NULL) {
        *next = '\0';
      }

      if (!strncmp(line, "audio:", strlen("audio:"))) {
        audio_line = line;
        break;
      }

      line = next ? next + 1 : NULL;
    }

    if (audio_line == NULL) {
      log_error(
          "Could not find 'audio' group in /etc/group required by libasound "
          "likely for defaults.pcm.ipc_gid"
          " from asound.conf. Make sure the group exists.");
      free(buffer);
      *result = NULL;
      return -1;
    }

    if (!patch_asound_fix_parse_group_line(audio_line, grp)) {
      log_error(
          "audio group entry format in /etc/group invalid. Check your "
          "/etc/group file!");
      free(buffer);
      *result = NULL;
      return -1;
    }

    char *grp_mem_str = patch_asound_fix_user_list_to_str(grp->gr_mem);
    log_debug(
        "Resulting group struct: %s %s %d %s",
        grp->gr_name,
        grp->gr_passwd,
        grp->gr_gid,
        grp_mem_str);
    free(grp_mem_str);
    free(buffer);

    *result = grp;

    return 0;
  }

  return patch_asound_fix_real_getgrnam_r(name, grp, buf, buflen, result);
}

void patch_asound_fix_init()
{
  log_info("Initialized");
}