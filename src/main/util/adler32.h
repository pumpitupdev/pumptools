#ifndef UTIL_ADLER32_H
#define UTIL_ADLER32_H

#include <stdint.h>
#include <stdlib.h>

/**
 * Calculate the adler32 checksum
 *
 * @param initval Initial value for calculation
 * @param input Input buffer to checksum
 * @param length Length of the input buffer
 * @return Adler32 value of input buffer
 */
uint32_t util_adler32_calc(uint32_t initval, const uint8_t* input, size_t length);

#endif