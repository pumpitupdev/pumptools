#ifndef UTIL_DEFS_H
#define UTIL_DEFS_H

#include <stdint.h>
#include <string.h>

#define FILENAME \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)
#define XSTRINGIFY(arg) STRINGIFY(arg)

#define lengthof(x) (sizeof(x) / sizeof(x[0]))

#define containerof(ptr, outer_t, member) \
  ((void *) (((uint8_t *) ptr) - offsetof(outer_t, member)))

#endif
