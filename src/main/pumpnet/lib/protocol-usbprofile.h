#pragma once

#include <stdint.h>

#define USBPROFILE_ENDPOINT "/usbprofile/v1"

struct pumpnet_lib_usbprofile_get_save_req {
    uint64_t trace_id;
    uint64_t machine_id;
    uint64_t player_ref_id;
} __attribute__((__packed__));

struct pumpnet_lib_usbprofile_get_save_resp {
    size_t size;
    uint8_t data[];
} __attribute__((__packed__));

struct pumpnet_lib_usbprofile_get_rank_req {
    uint64_t trace_id;
    uint64_t machine_id;
    uint64_t player_ref_id;
} __attribute__((__packed__));

struct pumpnet_lib_usbprofile_get_rank_resp {
    size_t size;
    uint8_t data[];
} __attribute__((__packed__));

struct pumpnet_lib_usbprofile_put_save_req {
    uint64_t trace_id;
    uint64_t machine_id;
    uint64_t player_ref_id;
    size_t size;
    uint8_t data[];
} __attribute__((__packed__));

struct pumpnet_lib_usbprofile_put_save_resp {

} __attribute__((__packed__));

struct pumpnet_lib_usbprofile_put_rank_req {
    uint64_t trace_id;
    uint64_t machine_id;
    uint64_t player_ref_id;
    size_t size;
    uint8_t data[];
} __attribute__((__packed__));

struct pumpnet_lib_usbprofile_put_rank_resp {

} __attribute__((__packed__));