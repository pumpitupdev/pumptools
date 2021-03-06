#pragma once

#include <stdint.h>
#include <stdlib.h>

uint8_t *util_base64_encode(const uint8_t *src, size_t len, size_t *out_len);

uint8_t *util_base64_decode(const uint8_t *src, size_t len, size_t *out_len);