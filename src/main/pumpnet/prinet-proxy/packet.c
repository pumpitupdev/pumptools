#include "pumpnet/prinet-proxy/packet.h"

#include "util/log.h"
#include "util/mem.h"
#include "util/rand.h"

struct pumpnet_prinet_proxy_packet_data {
    uint32_t sequence_counter;
    uint32_t packet_id;
    uint8_t data[];
} __attribute__((__packed__));

struct pumpnet_prinet_proxy_packet_data_keepalive {
    uint32_t unknown;
} __attribute__((__packed__));

static const size_t HEADER_SIZE = sizeof(uint32_t) + sizeof(uint8_t) * SEC_PRINET_NOUNCE_LEN;

static const uint32_t PACKET_KEEPALIVE_ID = 0x3000000;
static const uint32_t PACKET_KEEPALIVE_SIZE =
        sizeof(struct pumpnet_prinet_proxy_packet_data) +
        sizeof(struct pumpnet_prinet_proxy_packet_data_keepalive);

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

struct pumpnet_prinet_proxy_packet* pumpnet_prinet_proxy_packet_create_keepalive()
{
    struct pumpnet_prinet_proxy_packet* packet;
    struct pumpnet_prinet_proxy_packet_data* data;

    packet = pumpnet_prinet_proxy_packet_alloc_response(PACKET_KEEPALIVE_SIZE);

    util_rand_gen_bytes(packet->nounce, SEC_PRINET_NOUNCE_LEN);

    data = (struct pumpnet_prinet_proxy_packet_data*) &packet->data;
    // sequences always start with 1
    data->sequence_counter = 1;
    data->packet_id = PACKET_KEEPALIVE_ID;

    // the data field of the keepalive packet is never read client-side

    return packet;
}