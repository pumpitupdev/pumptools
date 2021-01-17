#include <sodium.h>

#include "pumpnet/prinet-proxy/packet.h"

#include "util/log.h"
#include "util/mem.h"

static const size_t HEADER_SIZE = sizeof(uint32_t) + sizeof(uint8_t) * SEC_PRINET_NOUNCE_LEN;

struct pumpnet_prinet_proxy_packet* pumpnet_prinet_proxy_packet_alloc(size_t data_len)
{
    struct pumpnet_prinet_proxy_packet* packet;

    packet = util_xmalloc(HEADER_SIZE + data_len);

    packet->length = HEADER_SIZE + data_len;

    return packet;
}

struct pumpnet_prinet_proxy_packet* pumpnet_prinet_proxy_packet_alloc_response(size_t data_len)
{
    // + SEC_PRINET_MACBYTES_LEN expected on response only.
    // Request does not have this included at the end of the game specific data
    return pumpnet_prinet_proxy_packet_alloc(data_len + SEC_PRINET_MACBYTES_LEN);
}

size_t pumpnet_prinet_proxy_packet_get_header_len()
{
    return HEADER_SIZE;
}

size_t pumpnet_prinet_proxy_packet_get_data_len(const struct pumpnet_prinet_proxy_packet* packet)
{
    log_assert(packet);

    if (packet->length < HEADER_SIZE) {
        log_error("Packet with size less than header size");
        return 0;
    }

    return packet->length - HEADER_SIZE;
}