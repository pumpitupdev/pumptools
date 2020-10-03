#ifndef CRYPT_AES_H
#define CRYPT_AES_H

#include <stdint.h>
#include <stdlib.h>

/**
 * AES enc/dec key lengths
 */
enum crypt_aes_key_length {
    /* 128 bit */
    CRYPT_AES_KEY_LENGTH_16_BYTES = 16,
    /* 192 bit */
    CRYPT_AES_KEY_LENGTH_24_BYTES = 24,
    /* 256 bit */
    CRYPT_AES_KEY_LENGTH_32_BYTES = 32,
};

/**
 * Encrypt data
 *
 * @param key AES key to use
 * @param key_len Length of the key
 * @param out Output buffer encrypted data
 * @param in Input buffer to encrypt
 * @param nbytes Number of bytes of input buffer to encrypt
 */
void crypt_aes_enc(uint8_t* key, enum crypt_aes_key_length key_len, 
    uint8_t* out, const uint8_t* in, size_t nbytes);

/**
 * Decrypt data
 *
 * @param key AES key to use
 * @param key_len Length of the key
 * @param out Output buffer for decrypted data
 * @param in Input buffer to decrypt
 * @param nbytes Number of bytes of input buffer to decrypt
 */
void crypt_aes_dec(uint8_t* key, enum crypt_aes_key_length key_len, 
    uint8_t* out, const uint8_t* in, size_t nbytes);

#endif