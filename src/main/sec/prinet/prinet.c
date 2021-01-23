#define LOG_MODULE "sec-prinet"

#include <errno.h>
#include <sodium.h>
#include <string.h>
#include <unistd.h>

#include "sec/prinet/prinet.h"

#include "util/log.h"
#include "util/mem.h"

_Static_assert (crypto_box_NONCEBYTES == SEC_PRINET_NOUNCE_LEN, "Nounce length not as expected for prinet");
_Static_assert (crypto_box_MACBYTES == SEC_PRINET_MACBYTES_LEN, "Nounce length not as expected for prinet");

static void* _key_pub;
static size_t _key_pub_len;
static void* _key_priv;
static size_t _key_priv_len;

static void* _sec_prinet_safe_malloc(size_t size)
{
    size_t pagesize = getpagesize();
    size_t num_pages = size / pagesize;
    size_t num_pages_round_up = num_pages;

    if (size % pagesize != 0) {
        num_pages_round_up++;
    }

    size_t size_aligned = num_pages_round_up * pagesize;

    void* ptr = sodium_malloc(size_aligned);

    if (ptr == NULL) {
        log_error("Sodium malloc failed, %s", strerror(errno));
        return NULL;
    }

    return ptr;
}

void sec_prinet_init(
        const uint8_t* key_pub, size_t key_pub_len,
        const uint8_t* key_priv, size_t key_priv_len)
{
    _key_pub = util_xmalloc(key_pub_len);
    memcpy(_key_pub, key_pub, key_pub_len);
    _key_pub_len = key_pub_len;

    _key_priv = util_xmalloc(key_priv_len);
    memcpy(_key_priv, key_priv, key_priv_len);
    _key_priv_len = key_priv_len;

    // 0 on success, 1 if already initialized
    if (sodium_init() == -1) {
        log_error("Initializing sodium failed");
    }
}

void sec_prinet_finit()
{
    util_xfree(&_key_pub);
    util_xfree(&_key_priv);
}

size_t sec_prinet_get_enc_data_buffer_size(size_t enc_data_len)
{
    return enc_data_len + crypto_box_MACBYTES;
}

size_t sec_prinet_decrypt(
        const uint8_t* nounce, size_t nounce_len, const uint8_t* enc_data,
        size_t enc_data_len, uint8_t* dec_data)
{
    if (nounce_len != SEC_PRINET_NOUNCE_LEN) {
        log_error("Nounce len %d does not match expected len %d", nounce_len, SEC_PRINET_NOUNCE_LEN);
        return -1;
    }

    if (crypto_box_open_easy(dec_data, enc_data, enc_data_len, nounce, _key_pub, _key_priv) != 0) {
        log_error("Decrypting message, len %d, failed", enc_data_len);
        return -1;
    }

    // cut off the macbytes at the end of the message
    return enc_data_len - crypto_box_MACBYTES;
}

size_t sec_prinet_encrypt(
        const uint8_t* nounce, size_t nounce_len, const uint8_t* dec_data,
        size_t dec_data_len, uint8_t* enc_data)
{
    if (nounce_len != SEC_PRINET_NOUNCE_LEN) {
        log_error("Nounce len %d does not match expected len %d", nounce_len, SEC_PRINET_NOUNCE_LEN);
        return -1;
    }

    void* tmp_enc = _sec_prinet_safe_malloc(dec_data_len);

    if (tmp_enc == NULL) {
        return -1;
    }

    if (crypto_box_easy(tmp_enc, dec_data, dec_data_len, nounce, _key_pub, _key_priv) != 0) {
        log_error("Encrypting message, len %d, failed", dec_data_len);
        sodium_free(tmp_enc);
        return -1;
    }

    // Encrypted data/response expects additional data after game specific payload
    size_t enc_data_len = sec_prinet_get_enc_data_buffer_size(dec_data_len);

    memcpy(enc_data, tmp_enc, enc_data_len);

    sodium_free(tmp_enc);

    return enc_data_len;
}