#define LOG_MODULE "sec-hasp"

#include <string.h>

#include "sec/hasp/old/hasp.h"

#include "util/log.h"
#include "util/mem.h"
#include "util/str.h"

struct sec_hasp_key_table {
  uint32_t nkeys;
  struct sec_hasp_key *keys;
};

/* index 0 (type): 1 = dev, 2 = retail
   index 1 (language): 3001 = english */
/* static const uint32_t sec_hasp_features[] = {2, 3001}; */
static const uint32_t sec_hasp_key_id = 0xDEADBEEF;
static struct sec_hasp_key_table sec_hasp_keys;

void sec_hasp_init(const uint8_t *key_data, size_t len)
{
  sec_hasp_keys.nkeys = len / sizeof(struct sec_hasp_key);
  sec_hasp_keys.keys =
      util_xmalloc(sizeof(struct sec_hasp_key) * sec_hasp_keys.nkeys);

  memcpy(
      sec_hasp_keys.keys,
      key_data,
      sec_hasp_keys.nkeys * sizeof(struct sec_hasp_key));

  log_info("Loaded %d keys", sec_hasp_keys.nkeys);
}

int sec_hasp_api_login(int feature, int vendor_code, int *handle)
{
  *handle = (int) &sec_hasp_key_id;

  log_debug(
      "login, feature %d, vendor_code %d, ret handle 0x%X",
      feature,
      vendor_code,
      *handle);

  return 0;
}

int sec_hasp_api_logout(int handle)
{
  if (handle != (int) &sec_hasp_key_id) {
    log_error("Logout of unknown handle 0x%X", handle);
  } else {
    log_debug("Logout: 0x%X", handle);
  }

  return 0;
}

unsigned int sec_hasp_api_getid(void)
{
  return sec_hasp_key_id;
}

int sec_hasp_api_decrypt(int handle, void *buffer, size_t length)
{
  char *buf;

  if (handle != (int) &sec_hasp_key_id) {
    log_error("Unknown handle 0x%X sending decrypt request, ignored");
    return -1;
  }

  for (uint32_t i = 0; i < sec_hasp_keys.nkeys; i++) {
    if (!memcmp(
            buffer,
            sec_hasp_keys.keys[i].req,
            sizeof(sec_hasp_keys.keys[i].req))) {
      log_debug(
          "Decrypt %u -> %u",
          *((uint64_t *) sec_hasp_keys.keys[i].req),
          *((uint64_t *) sec_hasp_keys.keys[i].resp));
      memcpy(
          buffer,
          sec_hasp_keys.keys[i].resp,
          sizeof(sec_hasp_keys.keys[i].resp));
      return 0;
    }
  }

  buf = util_str_buffer(buffer, length);

  log_warn("Missing key for request %s", buf);
  free(buf);

  return 0;
}