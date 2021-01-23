#pragma once

#include <stdint.h>

#include "pumpnet/prinet-proxy/client.h"

void pumpnet_prinet_proxy_keepalive_init(struct pumpnet_prinet_proxy_client_connection* connection, uint32_t poll_ms);

void pumpnet_prinet_proxy_keepalive_shutdown();