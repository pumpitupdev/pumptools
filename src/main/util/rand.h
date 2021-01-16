#ifndef UTIL_RAND_H
#define UTIL_RAND_H

#include <stdint.h>

/**
 * General note for this module: The implementations provided are supposed to
 * be used for non-security related randomness, e.g. generate some random data
 * for testing something.
 */

void util_rand_init(uint32_t seed);

uint8_t util_rand_gen_8();

uint16_t util_rand_gen_16();

uint32_t util_rand_gen_32();

uint32_t util_rand_gen_range_32(uint32_t max);

uint64_t util_rand_gen_64();

void util_rand_gen_bytes(uint8_t* buffer, size_t len);

#endif
