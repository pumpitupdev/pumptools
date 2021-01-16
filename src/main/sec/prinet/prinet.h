#pragma once

#include <stdint.h>
#include <stdlib.h>

#define SEC_PRINET_NOUNCE_LEN 24
#define SEC_PRINET_MACBYTES_LEN 16

void sec_prinet_init(
        const uint8_t* key_pub, size_t key_pub_len,
        const uint8_t* key_priv, size_t key_priv_len);

void sec_prinet_finit();

size_t sec_prinet_get_enc_data_buffer_size(size_t enc_data_len);

size_t sec_prinet_decrypt(
    const uint8_t* nounce, size_t nounce_len, const uint8_t* enc_data,
    size_t enc_data_len, uint8_t* dec_data);

size_t sec_prinet_encrypt(
    const uint8_t* nounce, size_t nounce_len, const uint8_t* dec_data,
    size_t dec_data_len, uint8_t* enc_data);