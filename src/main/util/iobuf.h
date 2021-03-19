#ifndef UTIL_IOBUF_H
#define UTIL_IOBUF_H

#include <stddef.h>
#include <stdint.h>

/**
 * Iobuffer struct holding data, size and position
 */
struct util_iobuf {
  uint8_t *bytes;
  size_t nbytes;
  size_t pos;
};

/**
 * Const variant of iobuffer struct holding data, size and position
 */
struct util_const_iobuf {
  const uint8_t *bytes;
  size_t nbytes;
  size_t pos;
};

/**
 * Move the contents of one iobuffer to another one (data, size and pos)
 *
 * @param dest Destination to move to
 * @param src Source to move from
 * @return Number of bytes moved (this can be less than the source's size
 *         e.g. if destination size < src size)
 */
size_t util_iobuf_move(struct util_iobuf *dest, struct util_const_iobuf *src);

#endif
