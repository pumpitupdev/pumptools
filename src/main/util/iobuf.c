#include <string.h>

#include "util/iobuf.h"
#include "util/log.h"
#include "util/mem.h"

void util_iobuf_alloc(struct util_iobuf* iobuf, size_t size)
{
    log_assert(iobuf);

    iobuf->bytes = util_xmalloc(size);
    iobuf->nbytes = size;
    iobuf->pos = 0;
}

void util_iobuf_alloc2(struct util_iobuf** iobuf, size_t size)
{
    log_assert(iobuf);

    iobuf = util_xmalloc(sizeof(struct util_iobuf));

    (*iobuf)->bytes = util_xmalloc(size);
    (*iobuf)->nbytes = size;
    (*iobuf)->pos = 0;
}

void util_const_iobuf_alloc(struct util_const_iobuf* iobuf, size_t size)
{
    log_assert(iobuf);

    iobuf->bytes = util_xmalloc(size);
    iobuf->nbytes = size;
    iobuf->pos = 0;
}

void util_const_iobuf_alloc2(struct util_const_iobuf** iobuf, size_t size)
{
    log_assert(iobuf);

    iobuf = util_xmalloc(sizeof(struct util_iobuf));

    (*iobuf)->bytes = util_xmalloc(size);
    (*iobuf)->nbytes = size;
    (*iobuf)->pos = 0;
}

void util_iobuf_free(struct util_iobuf* iobuf)
{
    log_assert(iobuf);

    util_xfree((void**) &iobuf->bytes);
    iobuf->nbytes = 0;
    iobuf->pos = 0;
}

void util_iobuf_free2(struct util_iobuf** iobuf)
{
    log_assert(iobuf);

    util_xfree((void**) &(*iobuf)->bytes);
    util_xfree((void**) &iobuf);
}

void util_const_iobuf_free(struct util_const_iobuf* iobuf)
{
    log_assert(iobuf);

    util_xfree((void**) &iobuf->bytes);
    iobuf->nbytes = 0;
    iobuf->pos = 0;
}

void util_const_iobuf_free2(struct util_const_iobuf** iobuf)
{
    log_assert(iobuf);

    util_xfree((void**) &(*iobuf)->bytes);
    util_xfree((void**) &iobuf);
}

size_t util_iobuf_move(struct util_iobuf* dest, struct util_const_iobuf* src)
{
    size_t dest_avail;
    size_t src_avail;
    size_t chunksz;

    dest_avail = dest->nbytes - dest->pos;
    src_avail = src->nbytes - src->pos;
    chunksz = dest_avail < src_avail ? dest_avail : src_avail;

    memcpy(&dest->bytes[dest->pos], &src->bytes[src->pos], chunksz);

    dest->pos += chunksz;
    src->pos += chunksz;

    return chunksz;
}

