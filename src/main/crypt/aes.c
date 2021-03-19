#define LOG_MODULE "crypt-aes"

#include "lib/rijndael.h"

#include "aes.h"

void crypt_aes_enc(
    uint8_t *key,
    enum crypt_aes_key_length key_len,
    uint8_t *out,
    const uint8_t *in,
    size_t nbytes)
{
  unsigned long rk[RKLENGTH(128)];
  int nrounds;

  nrounds = rijndaelSetupEncrypt(rk, key, key_len * 8);
  for (size_t i = 0; i < nbytes / key_len; i++) {
    rijndaelEncrypt(rk, nrounds, &in[i * key_len], &out[i * key_len]);
  }
}

void crypt_aes_dec(
    uint8_t *key,
    enum crypt_aes_key_length key_len,
    uint8_t *out,
    const uint8_t *in,
    size_t nbytes)
{
  unsigned long rk[RKLENGTH(128)];
  int nrounds;

  nrounds = rijndaelSetupDecrypt(rk, key, key_len * 8);
  for (size_t i = 0; i < nbytes / key_len; i++) {
    rijndaelDecrypt(rk, nrounds, &in[i * key_len], &out[i * key_len]);
  }
}