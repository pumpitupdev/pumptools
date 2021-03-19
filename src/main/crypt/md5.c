#define LOG_MODULE "crypt-md5"

#include "lib/md5.h"

#include "md5.h"

void crypt_md5_hash(uint8_t *out, const uint8_t *in, size_t nbytes)
{
  MD5_CTX ctx;

  MD5Init(&ctx);
  MD5Update(&ctx, (unsigned char *) in, (unsigned int) nbytes);
  MD5Final(out, &ctx);
}