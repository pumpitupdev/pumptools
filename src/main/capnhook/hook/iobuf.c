#include <assert.h>
#include <string.h>

#include "capnhook/hook/iobuf.h"

void cnh_iobuf_flip(struct cnh_const_iobuf *child, struct cnh_iobuf *parent)
{
    assert(child != NULL);
    assert(parent != NULL);

    child->bytes = parent->bytes;
    child->pos = 0;
    child->nbytes = parent->pos;
}

size_t cnh_iobuf_move(struct cnh_iobuf *dest, struct cnh_const_iobuf *src)
{
    size_t dest_avail;
    size_t src_avail;
    size_t chunksz;

    assert(dest != NULL);
    assert(dest->bytes != NULL || dest->nbytes == 0);
    assert(dest->pos <= dest->nbytes);

    assert(src != NULL);
    assert(src->bytes != NULL || src->nbytes == 0);
    assert(src->pos <= src->nbytes);

    dest_avail = dest->nbytes - dest->pos;
    src_avail = src->nbytes - src->pos;
    chunksz = dest_avail < src_avail ? dest_avail : src_avail;

    memcpy(&dest->bytes[dest->pos], &src->bytes[src->pos], chunksz);

    dest->pos += chunksz;
    src->pos += chunksz;

    return chunksz;
}

size_t cnh_iobuf_shift(struct cnh_iobuf *dest, struct cnh_iobuf *src)
{
    struct cnh_const_iobuf span;

    assert(dest != NULL);
    assert(src != NULL);

    cnh_iobuf_flip(&span, src);
    cnh_iobuf_move(dest, &span);

    memmove(src->bytes, &src->bytes[span.pos], span.nbytes - span.pos);
    src->pos -= span.pos;

    return span.pos;
}

enum cnh_result cnh_iobuf_read(struct cnh_const_iobuf *src, void *bytes, size_t nbytes)
{
    assert(src != NULL);
    assert(bytes != NULL || nbytes == 0);

    if (src->pos + nbytes > src->nbytes) {
        return CNH_RESULT_ERROR_INSUFFICIENT_BUFFER;
    }

    memcpy(bytes, &src->bytes[src->pos], nbytes);
    src->pos += nbytes;

    return CNH_RESULT_SUCCESS;
}

enum cnh_result cnh_iobuf_read_8(struct cnh_const_iobuf *src, uint8_t *out)
{
    assert(src != NULL);
    assert(out != NULL);

    if (src->pos + sizeof(uint8_t) > src->nbytes) {
        return CNH_RESULT_ERROR_INSUFFICIENT_BUFFER;
    }

    *out = src->bytes[src->pos++];

    return CNH_RESULT_SUCCESS;
}

enum cnh_result cnh_iobuf_read_be16(struct cnh_const_iobuf *src, uint16_t *out)
{
    uint16_t value;

    assert(src != NULL);
    assert(out != NULL);

    if (src->pos + sizeof(uint16_t) > src->nbytes) {
        return CNH_RESULT_ERROR_INSUFFICIENT_BUFFER;
    }

    value  = src->bytes[src->pos++] << 8;
    value |= src->bytes[src->pos++];

    *out = value;

    return CNH_RESULT_SUCCESS;
}

enum cnh_result cnh_iobuf_read_be32(struct cnh_const_iobuf *src, uint32_t *out)
{
    uint32_t value;

    assert(src != NULL);
    assert(out != NULL);

    if (src->pos + sizeof(uint32_t) > src->nbytes) {
        return CNH_RESULT_ERROR_INSUFFICIENT_BUFFER;
    }

    value  = src->bytes[src->pos++] << 24;
    value |= src->bytes[src->pos++] << 16;
    value |= src->bytes[src->pos++] << 8;
    value |= src->bytes[src->pos++];

    *out = value;

    return CNH_RESULT_SUCCESS;
}

enum cnh_result cnh_iobuf_read_le16(struct cnh_const_iobuf *src, uint16_t *out)
{
    uint16_t value;

    assert(src != NULL);
    assert(out != NULL);

    if (src->pos + sizeof(uint16_t) > src->nbytes) {
        return CNH_RESULT_ERROR_INSUFFICIENT_BUFFER;
    }

    value  = src->bytes[src->pos++];
    value |= src->bytes[src->pos++] << 8;

    *out = value;

    return CNH_RESULT_SUCCESS;
}

enum cnh_result cnh_iobuf_read_le32(struct cnh_const_iobuf *src, uint32_t *out)
{
    uint32_t value;

    assert(src != NULL);
    assert(out != NULL);

    if (src->pos + sizeof(uint32_t) > src->nbytes) {
        return CNH_RESULT_ERROR_INSUFFICIENT_BUFFER;
    }

    value  = src->bytes[src->pos++];
    value |= src->bytes[src->pos++] << 8;
    value |= src->bytes[src->pos++] << 16;
    value |= src->bytes[src->pos++] << 24;

    *out = value;

    return CNH_RESULT_SUCCESS;
}

enum cnh_result cnh_iobuf_write(struct cnh_iobuf *dest, const void *bytes, size_t nbytes)
{
    assert(dest != NULL);
    assert(bytes != NULL || nbytes == 0);

    if (dest->pos + nbytes > dest->nbytes) {
        return CNH_RESULT_ERROR_INSUFFICIENT_BUFFER;
    }

    memcpy(&dest->bytes[dest->pos], bytes, nbytes);
    dest->pos += nbytes;

    return CNH_RESULT_SUCCESS;
}

enum cnh_result cnh_iobuf_write_8(struct cnh_iobuf *dest, uint8_t value)
{
    assert(dest != NULL);

    if (dest->pos + sizeof(uint8_t) > dest->nbytes) {
        return CNH_RESULT_ERROR_INSUFFICIENT_BUFFER;
    }

    dest->bytes[dest->pos++] = value;

    return CNH_RESULT_SUCCESS;
}

enum cnh_result cnh_iobuf_write_be16(struct cnh_iobuf *dest, uint16_t value)
{
    assert(dest != NULL);

    if (dest->pos + sizeof(uint16_t) > dest->nbytes) {
        return CNH_RESULT_ERROR_INSUFFICIENT_BUFFER;
    }

    dest->bytes[dest->pos++] = (uint8_t) (value >> 8);
    dest->bytes[dest->pos++] = (uint8_t) value;

    return CNH_RESULT_SUCCESS;
}

enum cnh_result cnh_iobuf_write_be32(struct cnh_iobuf *dest, uint32_t value)
{
    assert(dest != NULL);

    if (dest->pos + sizeof(uint32_t) > dest->nbytes) {
        return CNH_RESULT_ERROR_INSUFFICIENT_BUFFER;
    }

    dest->bytes[dest->pos++] = (uint8_t) (value >> 24);
    dest->bytes[dest->pos++] = (uint8_t) (value >> 16);
    dest->bytes[dest->pos++] = (uint8_t) (value >> 8);
    dest->bytes[dest->pos++] = (uint8_t) value;

    return CNH_RESULT_SUCCESS;
}

enum cnh_result cnh_iobuf_write_le16(struct cnh_iobuf *dest, uint16_t value)
{
    assert(dest != NULL);

    if (dest->pos + sizeof(uint16_t) > dest->nbytes) {
        return CNH_RESULT_ERROR_INSUFFICIENT_BUFFER;
    }

    dest->bytes[dest->pos++] = (uint8_t) value;
    dest->bytes[dest->pos++] = (uint8_t) (value >> 8);

    return CNH_RESULT_SUCCESS;
}

enum cnh_result cnh_iobuf_write_le32(struct cnh_iobuf *dest, uint32_t value)
{
    assert(dest != NULL);

    if (dest->pos + sizeof(uint32_t) > dest->nbytes) {
        return CNH_RESULT_ERROR_INSUFFICIENT_BUFFER;
    }

    dest->bytes[dest->pos++] = (uint8_t) value;
    dest->bytes[dest->pos++] = (uint8_t) (value >> 8);
    dest->bytes[dest->pos++] = (uint8_t) (value >> 16);
    dest->bytes[dest->pos++] = (uint8_t) (value >> 24);

    return CNH_RESULT_SUCCESS;
}
