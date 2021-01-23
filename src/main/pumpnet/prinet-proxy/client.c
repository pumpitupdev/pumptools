#include <pthread.h>

#include "pumpnet/prinet-proxy/client.h"

#include "util/log.h"
#include "util/mem.h"
#include "util/sock-tcp.h"

struct pumpnet_prinet_proxy_client_connection {
    int handle;
    pthread_mutex_t mutex;
};

static const uint32_t _pumpnet_prinet_proxy_client_recv_length_timeout_ms = 500;

static bool _pumpnet_prinet_proxy_client_recv_packet_length(struct pumpnet_prinet_proxy_client_connection* connection, uint32_t* length)
{
    uint32_t packet_length;

    while (true) {
        ssize_t read;

        pthread_mutex_lock(&connection->mutex);

        read = util_sock_tcp_recv(connection->handle, &packet_length, sizeof(uint32_t), _pumpnet_prinet_proxy_client_recv_length_timeout_ms);

        pthread_mutex_unlock(&connection->mutex);

        if (read == 0) {
            // no data, repeat
            continue;
        }

        if (read != sizeof(uint32_t)) {
            return false;
        }

        break;
    }

    *length = packet_length;

    return true;
}

struct pumpnet_prinet_proxy_client_connection* pumpnet_prinet_proxy_client_connection_alloc()
{
    struct pumpnet_prinet_proxy_client_connection* connection;

    connection = util_xmalloc(sizeof(struct pumpnet_prinet_proxy_client_connection));

    connection->handle = INVALID_SOCK_HANDLE;
    pthread_mutex_init(&connection->mutex, NULL);

    return connection;
}

bool pumpnet_prinet_proxy_client_connection_accept(int socket, struct pumpnet_prinet_proxy_client_connection* connection)
{
    int handle;

    log_debug("Waiting for incoming connection...");

    handle = util_sock_tcp_wait_and_accept_remote_connection(socket);

    log_debug("Received connection on socket: %X", socket);

    if (socket == INVALID_SOCK_HANDLE) {
        log_error("Waiting and accepting source connection failed");
        return NULL;
    }

    pthread_mutex_lock(&connection->mutex);

    connection->handle = handle;

    log_debug("Received remote connection: %X", handle);

    pthread_mutex_unlock(&connection->mutex);

    return connection;
}

bool pumpnet_prinet_proxy_client_connection_is_active(struct pumpnet_prinet_proxy_client_connection* connection)
{
    log_assert(connection);

    bool is_active;

    pthread_mutex_lock(&connection->mutex);

    is_active = connection->handle != INVALID_SOCK_HANDLE;

    pthread_mutex_unlock(&connection->mutex);

    return is_active;
}

void pumpnet_prinet_proxy_client_connection_close(struct pumpnet_prinet_proxy_client_connection* connection)
{
    log_assert(connection);

    pthread_mutex_lock(&connection->mutex);

    log_debug("Closing remote connection: %X", connection->handle);

    if (connection->handle != INVALID_SOCK_HANDLE) {
        util_sock_tcp_close(connection->handle);

        connection->handle = INVALID_SOCK_HANDLE;
    }

    pthread_mutex_unlock(&connection->mutex);
}

void pumpnet_prinet_proxy_client_connection_free(struct pumpnet_prinet_proxy_client_connection** connection)
{
    log_assert(connection);

    struct pumpnet_prinet_proxy_client_connection* con = *connection;

    pthread_mutex_lock(&con->mutex);

    if (con->handle != INVALID_SOCK_HANDLE) {
        log_error("Freeing connection %X which is not closed", con->handle);
    }

    pthread_mutex_unlock(&con->mutex);

    pthread_mutex_destroy(&con->mutex);
    util_xfree((void**) connection);
}

struct pumpnet_prinet_proxy_packet* pumpnet_prinet_proxy_client_recv_request(struct pumpnet_prinet_proxy_client_connection* connection)
{
    log_assert(connection);

    uint32_t packet_length;

    // locking is handled inside this function
    if (!_pumpnet_prinet_proxy_client_recv_packet_length(connection, &packet_length)) {
        log_error("Receiving length field for request from source failed");
        return NULL;
    }

    pthread_mutex_lock(&connection->mutex);

    log_debug("Received length source request (%X): %d", connection->handle, packet_length);

    struct pumpnet_prinet_proxy_packet* packet = util_xmalloc(packet_length);
    packet->length = packet_length;

    size_t data_len = pumpnet_prinet_proxy_packet_get_data_len(packet);

    size_t read_pos = 0;

    while (true) {
        size_t remaining_size = packet->length - sizeof(uint32_t) - read_pos;

        ssize_t read = util_sock_tcp_recv_block(
            connection->handle,
            ((uint8_t*) packet) + sizeof(uint32_t) + read_pos,
            remaining_size);

        if (read == 0) {
            log_error("Unexpected remote close and no data");
            util_xfree((void**) &packet);
            break;
        }

        if (read == -1) {
            log_error("Receiving length field for request from source failed: %d", read);
            util_xfree((void**) &packet);
            break;
        }

        read_pos += read;

        if (read_pos >= data_len) {
            break;
        }
    }

    pthread_mutex_unlock(&connection->mutex);

    return packet;
}

bool pumpnet_prinet_proxy_client_send_response(struct pumpnet_prinet_proxy_client_connection* connection, const struct pumpnet_prinet_proxy_packet* packet)
{
    log_assert(connection);
    log_assert(packet);

    bool result;

    result = true;

    pthread_mutex_lock(&connection->mutex);

    log_debug("Sending source response (%X): %d", connection->handle, packet->length);

    if (util_sock_tcp_send_block(connection->handle, packet, packet->length) != packet->length) {
        log_error("Sending response, len %d, to source failed", packet->length);
        result = false;
    }

    pthread_mutex_unlock(&connection->mutex);

    return result;
}

bool pumpnet_prinet_proxy_client_unpack_data_request(const struct pumpnet_prinet_proxy_packet* packet, struct util_iobuf* dec_data)
{
    log_assert(packet);
    log_assert(dec_data);

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
    log_assert(req_packet);
    log_assert(dec_data);
    log_assert(resp_packet);

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
