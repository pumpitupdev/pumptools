#define LOG_MODULE "util-fs"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "util/defs.h"
#include "util/fs.h"
#include "util/log.h"
#include "util/mem.h"

bool util_file_load(
    const char *filename, void **out_bytes, size_t *out_nbytes, bool text_mode)
{
  FILE *f;
  void *bytes;
  size_t nbytes;
  int result;

  log_assert(out_bytes != NULL);
  *out_bytes = NULL;

  f = fopen(filename, "rb");

  if (f == NULL) {
    log_warn("%s: Error opening file: %s", filename, strerror(errno));

    goto open_fail;
  }

  fseek(f, 0, SEEK_END);
  nbytes = ftell(f);
  fseek(f, 0, SEEK_SET);

  /* Add for null terminator */
  if (text_mode) {
    nbytes++;
  }

  bytes = malloc(nbytes);

  if (bytes == NULL) {
    log_warn("%s: malloc(%u) failed", filename, (unsigned int) nbytes);

    goto malloc_fail;
  }

  if (text_mode) {
    result = fread(bytes, nbytes - 1, 1, f);
  } else {
    result = fread(bytes, nbytes, 1, f);
  }

  if (result != 1) {
    log_warn("%s: Error reading file: %s", filename, strerror(errno));

    goto read_fail;
  }

  *out_bytes = bytes;

  if (out_nbytes != NULL) {
    *out_nbytes = nbytes;
  }

  fclose(f);

  /* Add null terminator */
  if (text_mode) {
    ((char *) bytes)[nbytes - 1] = '\0';
  }

  log_debug("File loaded %s, size %d", filename, *out_nbytes);

  return true;

read_fail:
  free(bytes);

malloc_fail:
  fclose(f);

open_fail:
  return false;
}

bool util_file_save(const char *filename, const void *bytes, size_t nbytes)
{
  FILE *f;
  int result;

  f = fopen(filename, "wb");

  if (f == NULL) {
    log_warn("%s: Error creating file: %s", filename, strerror(errno));

    goto open_fail;
  }

  result = fwrite(bytes, nbytes, 1, f);

  if (result != 1) {
    log_warn("%s: Error writing file: %s", filename, strerror(errno));

    goto write_fail;
  }

  fclose(f);

  log_debug("File saved %s, size %d", filename, nbytes);

  return true;

write_fail:
  fclose(f);

open_fail:
  return false;
}

bool util_fs_path_exists(const char *path)
{
  // Don't use stat here because that doesn't work on mounts, e.g.
  // sqashfs/appimage

  bool exists;
  FILE *file = fopen(path, "r");

  exists = file != NULL;

  if (file != NULL) {
    fclose(file);
  }

  return exists;
}

bool util_fs_mkdir(const char *path)
{
  return mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
}

bool util_fs_mkfile(const char *path)
{
  FILE *file;
  bool res;

  file = fopen(path, "a");

  res = file != NULL;

  if (file) {
    fclose(file);
  }

  return res;
}

char *util_fs_get_path_to_file(const char *path)
{
  size_t len;
  char *res;

  len = strlen(path);

  for (ssize_t i = len - 1; i >= 0; i--) {
    if (path[i] == '/') {
      len--;
      break;
    }

    len--;
  }

  res = (char *) util_xmalloc(sizeof(char) * (len + 1));
  strncpy(res, path, len);
  res[len] = '\0';

  return res;
}

char *util_fs_get_filename(const char *path)
{
  size_t path_len;
  size_t len;
  char *res;

  len = strlen(path);
  path_len = len;

  for (ssize_t i = len - 1; i >= 0; i--) {
    if (path[i] == '/') {
      len++;
      break;
    }

    len--;
  }

  res = (char *) util_xmalloc(sizeof(char) * ((path_len - len) + 1));
  strncpy(res, path + len, path_len - len);
  res[path_len - len] = '\0';

  return res;
}

char *util_fs_get_abs_path(const char *path)
{
  char *real_path;

  real_path = realpath(path, NULL);

  if (!real_path) {
    log_error(
        "Resolving path '%s' to absolute path failed: %s",
        path,
        strerror(errno));
  }

  return real_path;
}