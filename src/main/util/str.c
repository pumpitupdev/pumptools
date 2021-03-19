#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/log.h"
#include "util/mem.h"
#include "util/str.h"

bool util_str_starts_with(const char *haystack, const char *needle)
{
  size_t haystack_len;
  size_t needle_len;

  haystack_len = strlen(haystack);
  needle_len = strlen(needle);

  if (needle_len > haystack_len) {
    return false;
  }

  return !memcmp(haystack, needle, needle_len);
}

bool util_str_ends_with(const char *haystack, const char *needle)
{
  size_t haystack_len;
  size_t needle_len;

  haystack_len = strlen(haystack);
  needle_len = strlen(needle);

  if (needle_len > haystack_len) {
    return false;
  }

  return !memcmp(&haystack[haystack_len - needle_len], needle, needle_len);
}

size_t util_str_format(char *buf, size_t nchars, const char *fmt, ...)
{
  va_list ap;
  size_t result;

  va_start(ap, fmt);
  result = util_str_vformat(buf, nchars, fmt, ap);
  va_end(ap);

  return result;
}

size_t util_str_vformat(char *buf, size_t nchars, const char *fmt, va_list ap)
{
  int result;

  result = vsnprintf(buf, nchars, fmt, ap);

  if (result >= (int) nchars || result < 0) {
    abort();
  }

  return (size_t) result;
}

void util_str_cat(char *dest, size_t dnchars, const char *src)
{
  size_t dlen;
  size_t slen;

  dlen = strlen(dest);
  slen = strlen(src);

  if (dlen + slen >= dnchars) {
    abort();
  }

  memcpy(dest + dlen, src, (slen + 1) * sizeof(char));
}

void util_str_cpy(char *dest, size_t dnchars, const char *src)
{
  size_t slen;

  slen = strlen(src);

  if (slen >= dnchars) {
    abort();
  }

  memcpy(dest, src, (slen + 1) * sizeof(char));
}

char *util_str_merge(const char *str1, const char *str2)
{
  char *out;
  size_t str1_len;
  size_t str2_len;
  size_t total;

  str1_len = strlen(str1);
  str2_len = strlen(str2);
  total = str1_len + str2_len + 1;

  out = util_xmalloc(total);
  memcpy(out, str1, str1_len);
  memcpy(out + str1_len, str2, str2_len);
  out[total - 1] = '\0';

  return out;
}

char *util_str_dup(const char *str)
{
  char *dest;
  size_t nbytes;

  log_assert(str);

  nbytes = strlen(str) + 1;
  dest = util_xmalloc(nbytes);
  memcpy(dest, str, nbytes);

  return dest;
}

void util_str_trim(char *str)
{
  char *pos;

  for (pos = str + strlen(str) - 1; pos > str; pos--) {
    if (!isspace(*pos)) {
      return;
    }

    *pos = '\0';
  }
}

char *util_str_buffer(const uint8_t *buf, size_t len)
{
  char *ret;
  size_t pos;

  pos = 0;
  ret = util_xmalloc(len * 3 + 1);

  for (size_t i = 0; i < len; i++) {
    uint8_t tmp = buf[i];

    pos += sprintf(ret + pos, "%02X ", tmp);
  }

  ret[len * 3] = '\0';

  return ret;
}

char **util_str_split(const char *str, const char *delim, size_t *count)
{
  char **toks;
  char *copy;
  char *cur;
  size_t counter;

  log_assert(str);
  log_assert(delim);
  log_assert(count);

  *count = 0;

  copy = util_str_dup(str);
  cur = strtok(copy, delim);

  // First iteration, just count items
  while (cur != NULL) {
    (*count)++;
    cur = strtok(NULL, delim);
  }

  util_xfree((void **) &copy);

  toks = util_xmalloc(sizeof(char *) * (*count));

  copy = util_str_dup(str);
  cur = strtok(copy, delim);

  counter = 0;

  // Second iteration, grab tokens
  while (cur != NULL) {
    toks[counter] = util_str_dup(cur);
    counter++;

    cur = strtok(NULL, delim);
  }

  util_xfree((void **) &copy);

  return toks;
}

void util_str_free_split(char **split, size_t count)
{
  log_assert(split);

  for (int i = 0; i < count; i++) {
    util_xfree((void **) &split[i]);
  }

  util_xfree((void **) &split);
}