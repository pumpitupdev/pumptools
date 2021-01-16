#ifndef UTIL_IOBUF_H
#define UTIL_IOBUF_H

#include <stddef.h>
#include <stdint.h>

/**
 * Iobuffer struct holding data, size and position
 */
struct util_iobuf {
    uint8_t* bytes;
    size_t nbytes;
    size_t pos;
};

/**
 * Const variant of iobuffer struct holding data, size and position
 */
struct util_const_iobuf {
    const uint8_t* bytes;
    size_t nbytes;
    size_t pos;
};

/**
 * Allocate and init a fresh iobuf struct
 *
 * @param iobuf Pointer to an already allocated struct, e.g. on the stack
 * @param size Size of the buffer
 */
void util_iobuf_alloc(struct util_iobuf* iobuf, size_t size);

/**
 * Allocate and init a fresh iobuf struct
 *
 * @param iobuf Pointer to a pointer, e.g. the resulting struct will be allcoated on the heap
 * @param size Size of the buffer
 */
void util_iobuf_alloc2(struct util_iobuf** iobuf, size_t size);

/**
 * Allocate and init a fresh const iobuf struct
 *
 * @param iobuf Pointer to an already allocated struct, e.g. on the stack
 * @param size Size of the buffer
 */
void util_const_iobuf_alloc(struct util_const_iobuf* iobuf, size_t size);

/**
 * Allocate and init a fresh const iobuf struct
 *
 * @param iobuf Pointer to a pointer, e.g. the resulting struct will be allcoated on the heap
 * @param size Size of the buffer
 */
void util_const_iobuf_alloc2(struct util_const_iobuf** iobuf, size_t size);

/**
 * Free an allocated iobuf struct
 *
 * @param iobuf Pointer to allocated iobuf struct, e.g. on stack
 */
void util_iobuf_free(struct util_iobuf* iobuf);

/**
 * Free an allocated iobuf struct
 *
 * @param iobuf Pointer to a pointer of an iobuf struct, e.g. iobuf on heap
 */
void util_iobuf_free2(struct util_iobuf** iobuf);

/**
 * Free an allocated const iobuf struct
 *
 * @param iobuf Pointer to allocated const iobuf struct, e.g. on stack
 */
void util_const_iobuf_free(struct util_const_iobuf* iobuf);

/**
 * Free an allocated const iobuf struct
 *
 * @param iobuf Pointer to a pointer of a const iobuf struct, e.g. iobuf on heap
 */
void util_const_iobuf_free2(struct util_const_iobuf** iobuf);

/**
 * Move the contents of one iobuffer to another one (data, size and pos)
 *
 * @param dest Destination to move to
 * @param src Source to move from
 * @return Number of bytes moved (this can be less than the source's size 
 *         e.g. if destination size < src size)
 */
size_t util_iobuf_move(struct util_iobuf* dest, struct util_const_iobuf* src);

#endif
