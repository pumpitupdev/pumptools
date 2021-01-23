#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "pumpnet/prinet-proxy/packet.h"

#include "util/iobuf.h"

struct pumpnet_prinet_proxy_packet* pumpnet_prinet_proxy_client_recv_request(int handle);

bool pumpnet_prinet_proxy_client_send_response(int handle, const struct pumpnet_prinet_proxy_packet* packet);

bool pumpnet_prinet_proxy_client_unpack_data_request(const struct pumpnet_prinet_proxy_packet* packet, struct util_iobuf* dec_data);

bool pumpnet_prinet_proxy_client_pack_data_response(const struct pumpnet_prinet_proxy_packet* req_packet, const struct util_iobuf* dec_data, struct pumpnet_prinet_proxy_packet* resp_packet);