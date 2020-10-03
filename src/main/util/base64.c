#include <ctype.h>

#include "util/base64.h"
#include "util/mem.h"

// Copy-pasted with minor tweaks from: https://github.com/littlstar/b64.c

static const char b64_table[] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
  'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
  'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
  'w', 'x', 'y', 'z', '0', '1', '2', '3',
  '4', '5', '6', '7', '8', '9', '+', '/'
};

uint8_t* util_base64_encode(const uint8_t* src, size_t len, size_t* out_len)
{
  int i = 0;
  int j = 0;
  unsigned char *enc = NULL;
  size_t size = 0;
  unsigned char buf[4];
  unsigned char tmp[3];

  enc = (unsigned char *) util_xmalloc(0);

  while (len--) {
    tmp[i++] = *(src++);

    if (3 == i) {
      buf[0] = (tmp[0] & 0xfc) >> 2;
      buf[1] = ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
      buf[2] = ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
      buf[3] = tmp[2] & 0x3f;

      enc = (unsigned char *) util_xrealloc(enc, size + 4);

      for (i = 0; i < 4; ++i) {
        enc[size++] = b64_table[buf[i]];
      }

      i = 0;
    }
  }

  if (i > 0) {
    for (j = i; j < 3; ++j) {
      tmp[j] = '\0';
    }

    buf[0] = (tmp[0] & 0xfc) >> 2;
    buf[1] = ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
    buf[2] = ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
    buf[3] = tmp[2] & 0x3f;

    for (j = 0; (j < i + 1); ++j) {
      enc = (unsigned char *) util_xrealloc(enc, size + 1);
      enc[size++] = b64_table[buf[j]];
    }

    while ((i++ < 3)) {
      enc = (unsigned char *) util_xrealloc(enc, size + 1);
      enc[size++] = '=';
    }
  }

  enc = (unsigned char *) util_xrealloc(enc, size + 1);
  enc[size] = '\0';

  *out_len = size;

  return enc;
}

uint8_t* util_base64_decode(const uint8_t* src, size_t len, size_t* out_len)
{
  int i = 0;
  int j = 0;
  int l = 0;
  size_t size = 0;
  unsigned char *dec = NULL;
  unsigned char buf[3];
  unsigned char tmp[4];

  dec = (unsigned char *) util_xmalloc(0);

  while (len--) {
    if ('=' == src[j]) {
        break;
    }
    if (!(isalnum(src[j]) || '+' == src[j] || '/' == src[j])) {
        break;
    }

    tmp[i++] = src[j++];

    if (4 == i) {
      for (i = 0; i < 4; ++i) {
        for (l = 0; l < 64; ++l) {
          if (tmp[i] == b64_table[l]) {
            tmp[i] = l;
            break;
          }
        }
      }

      buf[0] = (tmp[0] << 2) + ((tmp[1] & 0x30) >> 4);
      buf[1] = ((tmp[1] & 0xf) << 4) + ((tmp[2] & 0x3c) >> 2);
      buf[2] = ((tmp[2] & 0x3) << 6) + tmp[3];

      dec = (unsigned char *) util_xrealloc(dec, size + 3);

      if (dec != NULL){
        for (i = 0; i < 3; ++i) {
          dec[size++] = buf[i];
        }
      } else {
        return NULL;
      }

      i = 0;
    }
  }

  if (i > 0) {
    for (j = i; j < 4; ++j) {
      tmp[j] = '\0';
    }

    for (j = 0; j < 4; ++j) {
        for (l = 0; l < 64; ++l) {
          if (tmp[j] == b64_table[l]) {
            tmp[j] = l;
            break;
          }
        }
    }

    buf[0] = (tmp[0] << 2) + ((tmp[1] & 0x30) >> 4);
    buf[1] = ((tmp[1] & 0xf) << 4) + ((tmp[2] & 0x3c) >> 2);
    buf[2] = ((tmp[2] & 0x3) << 6) + tmp[3];

    dec = (unsigned char *)util_xrealloc(dec, size + (i - 1));

    if (dec != NULL){
      for (j = 0; (j < i - 1); ++j) {
        dec[size++] = buf[j];
      }
    } else {
      return NULL;
    }
  }

  dec = (unsigned char *)util_xrealloc(dec, size + 1);

  if (dec != NULL){
    dec[size] = '\0';
  } else {
    return NULL;
  }

  if (out_len != NULL) {
    *out_len = size;
  }

  return dec;
}