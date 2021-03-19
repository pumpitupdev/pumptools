#define LOG_MODULE "patch-asound-fix"

#include <errno.h>
#include <grp.h>

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

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = (char *) util_xmalloc(file_size);

    if (fread(buffer, file_size, 1, file) != 1) {
      log_error("Reading /etc/group file failed: %s", strerror(errno));
      free(buffer);
      *result = NULL;
      return -1;
    }

    char *str_audio_pos = strstr(buffer, name);

    if (str_audio_pos == NULL) {
      log_error(
          "Could not find 'audio' group in /etc/group required by libasound "
          "likely for defaults.pcm.ipc_gid"
          " from asound.conf. Make sure the group exists.");
      free(buffer);
      *result = NULL;
      return -1;
    }

    char *tok = strtok(str_audio_pos, ":");
    uint8_t cnt = 0;

    while (tok != NULL) {
      switch (cnt) {
        case 0:
          grp->gr_name = util_str_dup(tok);
          break;

        case 1:
          grp->gr_passwd = util_str_dup(tok);
          break;

        case 2:
          grp->gr_gid = strtol(tok, NULL, 10);
          break;

        case 3:
          grp->gr_mem = patch_asound_fix_split_user_list_str(tok);
          break;

        default:
          log_error(
              "audio group entry format in /etc/group invalid. Check your "
              "/etc/group file!");
          free(buffer);
          *result = NULL;
          return -1;
      }

      cnt++;
      tok = strtok(NULL, ":");
    }

    char *grp_mem_str = patch_asound_fix_user_list_to_str(grp->gr_mem);
    log_debug(
        "Resulting group struct: %s %s %d %s",
        grp->gr_name,
        grp->gr_passwd,
        grp->gr_gid,
        grp_mem_str);
    free(grp_mem_str);

    *result = grp;

    return 0;
  }

  return patch_asound_fix_real_getgrnam_r(name, grp, buf, buflen, result);
}

void patch_asound_fix_init()
{
  log_info("Initialized");
}