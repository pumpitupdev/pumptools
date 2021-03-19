#include "util/adler32.h"

#define BASE 65521

uint32_t
util_adler32_calc(uint32_t initval, const uint8_t *input, size_t length)
{
  unsigned int s1 = initval & 0xffff;
  unsigned int s2 = (initval >> 16) & 0xffff;
  unsigned int n;

  for (n = 0; n < length; n++) {
    s1 = (s1 + input[n]) % BASE;
    s2 = (s2 + s1) % BASE;
  }

  return (s2 << 16) + s1;
}