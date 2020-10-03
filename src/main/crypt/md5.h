#ifndef CRYPT_MD5_H
#define CRYPT_MD5_H

#include <stdint.h>
#include <stdlib.h>

/**
 * Create an MD5 hash
 *
 * @param out Output buffer
 * @param in Input buffer to hash
 * @param nbytes Number of bytes of input buffer to process
 */
void crypt_md5_hash(uint8_t* out, const uint8_t* in, size_t nbytes);

#endif