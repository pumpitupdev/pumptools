#define LOG_MODULE "patch-hasp"

#include "sec/hasp/old/hasp.h"

#include "util/log.h"
#include "util/patch.h"

static const uint8_t patch_hasp_login_sig[] = {
    0x00, 0xB8, 0x16, 0x00,
    0x00, 0x00, 0x8B, 0x4C,
    0x24, 0x14, 0x8B, 0x74,
    0x24, 0x10, 0x8B, 0x54
};

static const uint8_t patch_hasp_id_sig[] = {
    0x53, 0x83, 0xEC, 0x48,
    0xC7, 0x44, 0x24, 0x10
};

static const uint8_t patch_hasp_get_session_info_sig[] = {
   0x55, 0x57, 0x56, 0x53,
   0x83, 0xEC, 0x3C, 0xE8,
   0x00, 0x00, 0x00, 0x00,
   0x5B, 0x81, 0xC3, 0x98,
   0xA2, 0x10, 0x00, 0xBE,
   0x09, 0x00, 0x00, 0x00,
   0x8B, 0x7C, 0x24, 0x50,
   0x8B, 0x44, 0x24, 0x54
};

void patch_hasp_init(const uint8_t* key_data, size_t len)
{
    void* func_api_login;
    void* func_api_logout;
    void* func_api_decrypt;
    void* func_api_getid;
    void* func_api_get_session_info;

    func_api_login = util_patch_find_signiture(patch_hasp_login_sig,
        sizeof(patch_hasp_login_sig), -16, (void*) 0x80c0000, (void*) 0x80f0000,
        16);

    if (!func_api_login) {
        log_error("Could not find ApiLogin address");
        return;
    }

    log_debug("ApiLogin at %p", func_api_login);

    func_api_logout = (void*) (((size_t) func_api_login) - 0x130);
    log_debug("assuming ApiLogout at %p", func_api_logout);

    func_api_decrypt = (void*) (((size_t) func_api_login) - 0x200);
    log_debug("assuming ApiDecrypt at %p", func_api_decrypt);

    func_api_getid = util_patch_find_signiture(patch_hasp_id_sig,
        sizeof(patch_hasp_id_sig), 0, (void*) 0x8060000, (void*) 0x8090000,
        16);

    if (!func_api_getid) {
        log_error("Could not find ApiGetid address");
        return;
    }

    log_debug("ApiGetid at %p", func_api_getid);

    func_api_get_session_info = util_patch_find_signiture(
        patch_hasp_get_session_info_sig,
        sizeof(patch_hasp_get_session_info_sig),
        0,
        (void*) 0x8060000,
        (void*) 0x80F0000,
        16);

    if (!func_api_get_session_info) {
        log_error("Could not find ApiGetSessionInfo address");
        return;
    }

    log_debug("ApiGetSessionInfo at %p", func_api_get_session_info);

    util_patch_function((uintptr_t) func_api_login, sec_hasp_api_login);
    util_patch_function((uintptr_t) func_api_logout, sec_hasp_api_logout);
    util_patch_function((uintptr_t) func_api_decrypt, sec_hasp_api_decrypt);
    util_patch_function((uintptr_t) func_api_getid, sec_hasp_api_getid);
    util_patch_function((uintptr_t) func_api_get_session_info,
        sec_hasp_api_get_session_info);

    sec_hasp_init(key_data, len);

    log_info("Initialized");
}