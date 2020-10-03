#include <time.h>
#include <stdlib.h>

#include "rand.h"

static int _util_rand_init = 0;

static void _util_rand_initialize()
{
    if (!_util_rand_init) {
        _util_rand_init = 1;
        srand((unsigned int) time(NULL));
    }
}

uint32_t util_rand_gen_32()
{
    _util_rand_initialize();

    return (uint32_t) rand();
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