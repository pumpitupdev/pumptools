#pragma once

#include <stdint.h>
#include <stdlib.h>

#define PUMPNET_LIB_PRINET_ENDPOINT "/prinet/v1"

struct pumpnet_lib_prinet_req {
    uint64_t trace_id;
    uint64_t machine_id;
    uint32_t size;
    uint8_t data[];
} __attribute__((__packed__));

struct pumpnet_lib_prinet_resp {
    uint32_t size;
    uint8_t data[];
} __attribute__((__packed__));