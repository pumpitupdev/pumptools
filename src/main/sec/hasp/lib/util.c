#include <time.h>

#include "util/rand.h"

#include "util.h"

uint32_t sec_hasp_util_make_fake_handle_value()
{
    return util_rand_gen_32();
}

uint32_t sec_hasp_util_get_timestamp()
{
    return (uint32_t) time(NULL);
}