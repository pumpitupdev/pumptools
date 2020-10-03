#include "util/iobuf.h"

#include <string.h>

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

