#define LOG_MODULE "patch-hasp"

#include "sec/hasp/old/hasp.h"

#include "util/log.h"
#include "util/patch.h"

static const uint8_t patch_hasp_login_sig[] = {
    0x00,
    0xB8,
    0x16,
    0x00,
    0x00,
    0x00,
    0x8B,
    0x4C,
    0x24,
    0x14,
    0x8B,
    0x74,
    0x24,
    0x10,
    0x8B,
    0x54};

static const uint8_t patch_hasp_id_sig[] = {
    0x53, 0x83, 0xEC, 0x48, 0xC7, 0x44, 0x24, 0x10};

void patch_hasp_init(const uint8_t *key_data, size_t len)
{
  void *func_api_login;
  void *func_api_logout;
  void *func_api_decrypt;
  void *func_api_getid;

  func_api_login = util_patch_find_signiture(
      patch_hasp_login_sig,
      sizeof(patch_hasp_login_sig),
      -16,
      (void *) 0x80c0000,
      (void *) 0x80f0000,
      16);

  if (!func_api_login) {
    log_error("Could not find ApiLogin address");
    return;
  }

  log_debug("ApiLogin at %p", func_api_login);

  func_api_logout = (void *) (((size_t) func_api_login) - 0x130);
  log_debug("assuming ApiLogout at %p", func_api_logout);

  func_api_decrypt = (void *) (((size_t) func_api_login) - 0x200);
  log_debug("assuming ApiDecrypt at %p", func_api_decrypt);

  func_api_getid = util_patch_find_signiture(
      patch_hasp_id_sig,
      sizeof(patch_hasp_id_sig),
      0,
      (void *) 0x8060000,
      (void *) 0x8090000,
      16);

  if (!func_api_getid) {
    log_error("Could not find ApiGetid address");
    return;
  }

  log_debug("ApiGetid at %p", func_api_getid);

  util_patch_function((uintptr_t) func_api_login, sec_hasp_api_login);
  util_patch_function((uintptr_t) func_api_logout, sec_hasp_api_logout);
  util_patch_function((uintptr_t) func_api_decrypt, sec_hasp_api_decrypt);
  util_patch_function((uintptr_t) func_api_getid, sec_hasp_api_getid);

  sec_hasp_init(key_data, len);

  log_info("Initialized");
}