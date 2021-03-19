#include "ptapi/io/piubtn/keyboard-util/keyboard-conf.h"

#include "util/fs.h"
#include "util/log.h"
#include "util/mem.h"

const char *ptapi_io_piubtn_keyboard_util_conf_input_str
    [PTAPI_IO_PIUBTN_KEYBOARD_UTIL_CONF_INPUT_COUNT] = {
        "invalid",
        "P1 Left",
        "P1 Right",
        "P1 Start",
        "P1 Back",
        "P2 Left",
        "P2 Right",
        "P2 Start",
        "P2 Back",
};

void ptapi_io_piubtn_keyboard_util_conf_new(
    struct ptapi_io_piubtn_keyboard_util_conf **conf)
{
  log_assert(conf);

  *conf = util_xmalloc(sizeof(struct ptapi_io_piubtn_keyboard_util_conf));
  memset(*conf, 0, sizeof(struct ptapi_io_piubtn_keyboard_util_conf));
}

bool ptapi_io_piubtn_keyboard_util_conf_read_from_file(
    struct ptapi_io_piubtn_keyboard_util_conf **conf, const char *path)
{
  log_assert(path);
  log_assert(conf);

  void *buffer;
  size_t size;

  if (!util_file_load(path, &buffer, &size, false)) {
    return false;
  }

  *conf = (struct ptapi_io_piubtn_keyboard_util_conf *) buffer;

  return true;
}

bool ptapi_io_piubtn_keyboard_util_conf_write_to_file(
    struct ptapi_io_piubtn_keyboard_util_conf *conf, const char *path)
{
  log_assert(path);
  log_assert(conf);

  return util_file_save(
      path, conf, sizeof(struct ptapi_io_piubtn_keyboard_util_conf));
}