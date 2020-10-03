/**
 * Implementation of an I/O buffer used by hooking modules
 *
 * Based on original capnhook code for windows by decafcode:
 * https://github.com/decafcode/capnhook
 */
#ifndef CAPNHOOK_IOBUF_H
#define CAPNHOOK_IOBUF_H

#include <stddef.h>
#include <stdint.h>

#include "capnhook/hook/result.h"

/**
 * I/O buffer
 */
struct cnh_iobuf {
    uint8_t *bytes;
    size_t nbytes;
    size_t pos;
};

/**
 * I/O buffer
 */
struct cnh_const_iobuf {
    const uint8_t *bytes;
    size_t nbytes;
    size_t pos;
};

/**
 * Flip the I/O buffer. Set the current pos as size and reset the position.
 *
 * @param child Flipped I/O buffer
 * @param parent I/O buffer to flip
 */
void cnh_iobuf_flip(struct cnh_const_iobuf *child, struct cnh_iobuf *parent);

/**
 * Move the contents currently enclosed by pos and end of an I/O buffer to a destination.
 *
 * @param dest Destination buffer to move the source to, starting at current pos
 * @param src Source to move to the destination, starting at current pos up to end of the buffer
 * @return Number of bytes copied to destination
 */
size_t cnh_iobuf_move(struct cnh_iobuf *dest, struct cnh_const_iobuf *src);

size_t cnh_iobuf_shift(struct cnh_iobuf *dest, struct cnh_iobuf *src);

/**
 * Read data from an I/O buffer
 *
 * @param src Source to read from (starting at current position)
 * @param bytes Target to read to
 * @param nbytes Number of bytes to read
 * @return Result of operation
 */
enum cnh_result cnh_iobuf_read(struct cnh_const_iobuf *src, void *bytes, size_t nbytes);

/**
 * Read a byte from an I/O buffer
 *
 * @param src Source to read from (starting at current position)
 * @param value Pointer to variable to read the data into
 * @return Result of operation
 */
enum cnh_result cnh_iobuf_read_8(struct cnh_const_iobuf *src, uint8_t *value);

/**
 * Read a 2 byte value, big endian byte order, from an I/O buffer
 *
 * @param src Source to read from (starting at current position)
 * @param value Pointer to variable to read the data into
 * @return Result of operation
 */
enum cnh_result cnh_iobuf_read_be16(struct cnh_const_iobuf *src, uint16_t *value);

/**
 * Read a 4 byte value, big endian byte order, from an I/O buffer
 *
 * @param src Source to read from (starting at current position)
 * @param value Pointer to variable to read the data into
 * @return Result of operation
 */
enum cnh_result cnh_iobuf_read_be32(struct cnh_const_iobuf *src, uint32_t *value);

/**
 * Read a 2 byte value, little endian byte order, from an I/O buffer
 *
 * @param src Source to read from (starting at current position)
 * @param value Pointer to variable to read the data into
 * @return Result of operation
 */
enum cnh_result cnh_iobuf_read_le16(struct cnh_const_iobuf *src, uint16_t *value);

/**
 * Read a 4 byte value, little endian byte order, from an I/O buffer
 *
 * @param src Source to read from (starting at current position)
 * @param value Pointer to variable to read the data into
 * @return Result of operation
 */
enum cnh_result cnh_iobuf_read_le32(struct cnh_const_iobuf *src, uint32_t *value);

/**
 * Write data to an I/O buffer
 *
 * @param dest I/O buffer to write to (starting at current position)
 * @param bytes Pointer to data to write
 * @param nbytes Number of bytes to write
 * @return Result of operation
 */
enum cnh_result cnh_iobuf_write(struct cnh_iobuf *dest, const void *bytes, size_t nbytes);

/**
 * Write a byte to an I/O buffer
 *
 * @param dest Destination I/O buffer to write to (starting at current position)
 * @param value Value to write
 * @return Result of operation
 */
enum cnh_result cnh_iobuf_write_8(struct cnh_iobuf *dest, uint8_t value);

/**
 * Write a 2 byte value, big endian byte order, to an I/O buffer
 *
 * @param dest Destination I/O buffer to write to (starting at current position)
 * @param value Value to write
 * @return Result of operation
 */
enum cnh_result cnh_iobuf_write_be16(struct cnh_iobuf *dest, uint16_t value);

/**
 * Write a 4 byte value, big endian byte order, to an I/O buffer
 *
 * @param dest Destination I/O buffer to write to (starting at current position)
 * @param value Value to write
 * @return Result of operation
 */
enum cnh_result cnh_iobuf_write_be32(struct cnh_iobuf *dest, uint32_t value);

/**
 * Write a 2 byte value, little endian byte order, to an I/O buffer
 *
 * @param dest Destination I/O buffer to write to (starting at current position)
 * @param value Value to write
 * @return Result of operation
 */
enum cnh_result cnh_iobuf_write_le16(struct cnh_iobuf *dest, uint16_t value);

/**
 * Write a 4 byte value, little endian byte order, to an I/O buffer
 *
 * @param dest Destination I/O buffer to write to (starting at current position)
 * @param value Value to write
 * @return Result of operation
 */
enum cnh_result cnh_iobuf_write_le32(struct cnh_iobuf *dest, uint32_t value);

#endif
