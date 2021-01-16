#include <time.h>
#include <stdlib.h>

#include "util/log.h"
#include "util/rand.h"

static int _util_rand_init = 0;

static void _util_rand_initialize()
{
    if (!_util_rand_init) {
        _util_rand_init = 1;
        srand((unsigned int) time(NULL));
    }
}

void util_rand_init(uint32_t seed)
{
    _util_rand_init = 1;
    srand(seed);
}

uint8_t util_rand_gen_8()
{
    return (uint8_t) util_rand_gen_32();
}

uint16_t util_rand_gen_16()
{
    return (uint16_t) util_rand_gen_32();
}

uint32_t util_rand_gen_32()
{
    _util_rand_initialize();

    return (uint32_t) rand();
}

uint32_t util_rand_gen_range_32(uint32_t max)
{
    return util_rand_gen_32() % max;
}

uint64_t util_rand_gen_64()
{
    uint64_t value;

    _util_rand_initialize();

    value = 0;
    value |= (((uint64_t) rand()) << 32);
    value |= ((uint64_t) rand());

    return value;
}

void util_rand_gen_bytes(uint8_t* buffer, size_t len)
{
    log_assert(buffer);

    _util_rand_initialize();

    for (size_t i = 0; i < len; i++) {
        buffer[i] = (uint8_t) rand();
    }
}