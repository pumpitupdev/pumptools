#pragma once

#include <stdint.h>
#include <stdlib.h>

#include "sec/prinet/prinet.h"

struct pumpnet_prinet_proxy_packet {
    uint32_t length;
    uint8_t nounce[SEC_PRINET_NOUNCE_LEN];
    uint8_t data[];
} __attribute__((__packed__));

struct pumpnet_prinet_proxy_packet* pumpnet_prinet_proxy_packet_alloc(size_t data_len);

struct pumpnet_prinet_proxy_packet* pumpnet_prinet_proxy_packet_alloc_response(size_t data_len);

size_t pumpnet_prinet_proxy_packet_get_header_len();

size_t pumpnet_prinet_proxy_packet_get_data_len(const struct pumpnet_prinet_proxy_packet* packet);

struct pumpnet_prinet_proxy_packet* pumpnet_prinet_proxy_packet_create_keepalive();