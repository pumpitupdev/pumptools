#define LOG_MODULE "prinet-net-crypt"

#include "capnhook/hook/lib.h"

#include "util/log.h"

typedef int (*crypto_box_open_easy_t)(
        unsigned char* m,
        const unsigned char* c,
        unsigned long long clen,
        unsigned long long unkn_len,
        const unsigned char* n,
        const unsigned char* pk,
        const unsigned char* sk);

static crypto_box_open_easy_t prihook_net_crypt_real_crypto_box_open_easy;

int crypto_box_open_easy(
        unsigned char* m,
        const unsigned char* c,
        unsigned long long clen,
        unsigned long long unkn_len,
        const unsigned char* n,
        const unsigned char* pk,
        const unsigned char* sk)
{
    if (!prihook_net_crypt_real_crypto_box_open_easy) {
        prihook_net_crypt_real_crypto_box_open_easy =
            (crypto_box_open_easy_t) cnh_lib_get_func_addr("crypto_box_open_easy");
    }

    int res = prihook_net_crypt_real_crypto_box_open_easy(
        m, c, clen, unkn_len, n, pk, sk);

    if (res != 0) {
        log_error("Decrypting message (len %d) failed: %d", clen, res);
    }

    return res;
}
