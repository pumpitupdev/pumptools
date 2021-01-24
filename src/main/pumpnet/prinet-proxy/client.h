#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "pumpnet/prinet-proxy/packet.h"

#include "util/iobuf.h"

struct pumpnet_prinet_proxy_client_connection;

struct pumpnet_prinet_proxy_client_connection* pumpnet_prinet_proxy_client_connection_alloc();

bool pumpnet_prinet_proxy_client_connection_accept(int socket, struct pumpnet_prinet_proxy_client_connection* connection);

bool pumpnet_prinet_proxy_client_connection_is_active(struct pumpnet_prinet_proxy_client_connection* connection);

void pumpnet_prinet_proxy_client_connection_close(struct pumpnet_prinet_proxy_client_connection* connection);

void pumpnet_prinet_proxy_client_connection_free(struct pumpnet_prinet_proxy_client_connection** connection);

void pumpnet_prinet_proxy_client_exit_recv_request_blocking();

struct pumpnet_prinet_proxy_packet* pumpnet_prinet_proxy_client_recv_request(struct pumpnet_prinet_proxy_client_connection* connection);

bool pumpnet_prinet_proxy_client_send_response(struct pumpnet_prinet_proxy_client_connection* connection, const struct pumpnet_prinet_proxy_packet* packet);

bool pumpnet_prinet_proxy_client_unpack_data_request(const struct pumpnet_prinet_proxy_packet* packet, struct util_iobuf* dec_data);

bool pumpnet_prinet_proxy_client_pack_data_response(const struct pumpnet_prinet_proxy_packet* req_packet, const struct util_iobuf* dec_data, struct pumpnet_prinet_proxy_packet* resp_packet);