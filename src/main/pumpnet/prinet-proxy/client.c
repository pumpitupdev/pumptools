#include "pumpnet/prinet-proxy/client.h"

#include "util/log.h"
#include "util/mem.h"
#include "util/sock-tcp.h"

static bool _pumpnet_prinet_proxy_client_recv_packet_length(int handle, uint32_t* length)
{
    uint32_t packet_length;

    while (true) {
        ssize_t read = util_sock_tcp_recv_block(handle, &packet_length, sizeof(uint32_t));

        if (read == 0) {
            log_error("Unexpected remote close and no data");
            return NULL;
        }

        if (read != sizeof(uint32_t)) {
            return false;
        }

        break;
    }

    *length = packet_length;

    return true;
}

struct pumpnet_prinet_proxy_packet* pumpnet_prinet_proxy_client_recv_request(int handle)
{
    uint32_t packet_length;

    if (!_pumpnet_prinet_proxy_client_recv_packet_length(handle, &packet_length)) {
        log_error("Receiving length field for request from source failed");
        return NULL;
    }

    log_debug("Received length source request (%X): %d", handle, packet_length);

    struct pumpnet_prinet_proxy_packet* packet = util_xmalloc(packet_length);
    packet->length = packet_length;

    size_t data_len = pumpnet_prinet_proxy_packet_get_data_len(packet);

    size_t read_pos = 0;

    while (true) {
        size_t remaining_size = packet->length - sizeof(uint32_t) - read_pos;

        ssize_t read = util_sock_tcp_recv_block(
            handle,
            ((uint8_t*) packet) + sizeof(uint32_t) + read_pos,
            remaining_size);

        if (read == 0) {
            log_error("Unexpected remote close and no data");
            return NULL;
        }

        if (read == -1) {
            log_error("Receiving length field for request from source failed: %d", read);
            return NULL;
        }

        read_pos += read;

        if (read_pos >= data_len) {
            break;
        }
    }

    return packet;
}

bool pumpnet_prinet_proxy_client_send_response(int handle, const struct pumpnet_prinet_proxy_packet* packet)
{
    log_debug("Sending source response (%X): %d", handle, packet->length);

    if (util_sock_tcp_send_block(handle, packet, packet->length) != packet->length) {
        log_error("Sending response, len %d, to source failed", packet->length);
        return false;
    } else {
        return true;
    }
}

bool pumpnet_prinet_proxy_client_unpack_data_request(const struct pumpnet_prinet_proxy_packet* packet, struct util_iobuf* dec_data)
{
    size_t enc_data_len = pumpnet_prinet_proxy_packet_get_data_len(packet);

    size_t dec_data_len = sec_prinet_decrypt(
        packet->nounce,
        sizeof(packet->nounce),
        packet->data,
        enc_data_len,
        dec_data->bytes);

    if (dec_data_len < 0) {
        log_error("Decrypting data failed");
        return false;
    }

    dec_data->pos = dec_data_len;

    return true;
}

bool pumpnet_prinet_proxy_client_pack_data_response(const struct pumpnet_prinet_proxy_packet* req_packet, const struct util_iobuf* dec_data, struct pumpnet_prinet_proxy_packet* resp_packet)
{
    size_t enc_data_len = sec_prinet_encrypt(
        req_packet->nounce,
        sizeof(req_packet->nounce),
        dec_data->bytes,
        dec_data->pos,
        resp_packet->data);

    if (enc_data_len < 0) {
        log_error("Encrypting data failed");
        return false;
    }

    memcpy(resp_packet->nounce, req_packet->nounce, sizeof(resp_packet->nounce));

    resp_packet->length = pumpnet_prinet_proxy_packet_get_header_len() + enc_data_len;

    return true;
}