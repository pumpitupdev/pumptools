#include <stdatomic.h>
#include <signal.h>
#include <stdbool.h>

#include "pumpnet/lib/prinet.h"
#include "pumpnet/prinet-proxy/client.h"
#include "pumpnet/prinet-proxy/keepalive.h"
#include "pumpnet/prinet-proxy/packet.h"
#include "pumpnet/prinet-proxy/options.h"
#include "sec/prinet/prinet.h"

#include "util/iobuf.h"
#include "util/log.h"
#include "util/mem.h"
#include "util/sock-tcp.h"

// Since handling multiple connection requires multiple threads with sync
// receives here (see notes about bad connection management below), allowing
// more than one connection in the queue doesn't make sense
static const uint32_t NUM_CONNECTIONS = 1;
static const size_t PUMPNET_MAX_RESP_SIZE = 1024 * 1024;

static atomic_int _source_sock;

/* Compiled binary data from data folder. Symbols are defined by compiler */
extern const uint8_t _binary_prime_private_key_start[];
extern const uint8_t _binary_prime_private_key_end[];
extern const uint8_t _binary_prime_public_key_start[];
extern const uint8_t _binary_prime_public_key_end[];

static void _sigint_handler(int sig)
{
    log_info("SIGINT, exiting");

    util_sock_tcp_close(_source_sock);

    sec_prinet_finit();

    pumpnet_lib_prinet_shutdown();

    exit(EXIT_SUCCESS);
}

int main(int argc, char** argv)
{
    util_log_set_level(LOG_LEVEL_DEBUG);

    struct pumpnet_prinet_proxy_options options;

    if (!pumpnet_prinet_proxy_options_init(argc, argv, &options)) {
        return -1;
    }

    util_log_set_file(options.log.file, false);
    util_log_set_level(options.log.level);

    pumpnet_lib_prinet_init(
        options.proxy.pumpnet.game,
        options.proxy.pumpnet.server,
        options.proxy.pumpnet.machine_id,
        options.proxy.pumpnet.cert_dir_path,
        options.proxy.pumpnet.verbose_log_output);

    log_info("Proxy, source (listen/sink) %s:%d -> pumpnet server %s (game: %d)",
        options.proxy.source.addr,
        options.proxy.source.port,
        options.proxy.pumpnet.server,
        options.proxy.pumpnet.game);

    sec_prinet_init(
        (const uint8_t*) _binary_prime_public_key_start,
        ((uintptr_t) &_binary_prime_public_key_end -
        (uintptr_t) &_binary_prime_public_key_start),
        (const uint8_t*) _binary_prime_private_key_start,
        ((uintptr_t) &_binary_prime_private_key_end -
        (uintptr_t) &_binary_prime_private_key_start));

    _source_sock = util_sock_tcp_open_bind_listen2(
        options.proxy.source.addr,
        options.proxy.source.port,
        NUM_CONNECTIONS);

    if (_source_sock == INVALID_SOCK_HANDLE) {
        return -3;
    }

    log_info("Running proxy loop");

    signal(SIGINT, _sigint_handler);

    // Bad network plumbing on the game/client side:
    // The game expects to leave the connection open once established on the
    // first request to the server. A server side heartbeat package helps or
    // even ensures (depending how badly you implemented this) to detect if the
    // connection is dead and can be closed server side this does not just
    // hinder scalability server side regarding the number of connects that
    // might need to be kept open if potentially thousands of clients are
    // active at the same time, but also makes the entire connection layer of
    // the plumbing absolutely awful to maintain.
    // i guess the devs never expected to see more than a few hundred clients
    // active at the same time~

    while (true) {
        int source_con = INVALID_SOCK_HANDLE;

        log_debug("Waiting for incoming connection...");

        source_con = util_sock_tcp_wait_and_accept_remote_connection(_source_sock);

        log_debug("Received connection: %X", source_con);

        if (source_con == INVALID_SOCK_HANDLE) {
            log_error("Waiting and accepting source connection failed");
            continue;
        }

        bool inner_loop;

        inner_loop = true;

        do {
            struct pumpnet_prinet_proxy_packet* source_req = NULL;
            struct util_iobuf pumpnet_data_req;
            struct util_iobuf pumpnet_data_resp;
            struct pumpnet_prinet_proxy_packet* source_resp = NULL;

            source_req = pumpnet_prinet_proxy_client_recv_request(source_con);

            if (!source_req) {
                log_error("Receiving request from source failed");
                inner_loop = false;
                goto cleanup_iteration;
            }

            size_t source_data_len = pumpnet_prinet_proxy_packet_get_data_len(source_req);
            util_iobuf_alloc(&pumpnet_data_req, source_data_len);

            if (!pumpnet_prinet_proxy_client_unpack_data_request(source_req, &pumpnet_data_req)) {
                log_error("Transforming data request for destination failed");
                inner_loop = false;
                goto cleanup_iteration;
            }

            util_iobuf_alloc(&pumpnet_data_resp, PUMPNET_MAX_RESP_SIZE);

            ssize_t pumpnet_recv_size = pumpnet_lib_prinet_msg(
                pumpnet_data_req.bytes,
                pumpnet_data_req.pos,
                pumpnet_data_resp.bytes,
                pumpnet_data_resp.nbytes);

            // Uni-directional message, no response received
            // Skip response processing because there is nothing to process
            if (pumpnet_recv_size == -1) {
                log_debug("Request was uni-directional message, skipping response");
                goto cleanup_iteration;
            }

            if (pumpnet_recv_size < 0) {
                log_error("Request to pumpnet failed");
                inner_loop = false;
                goto cleanup_iteration;
            }

            // Communicate actual data size
            pumpnet_data_resp.pos = pumpnet_recv_size;

            source_resp = pumpnet_prinet_proxy_packet_alloc_response(pumpnet_data_resp.pos);

            if (!pumpnet_prinet_proxy_client_pack_data_response(source_req, &pumpnet_data_resp, source_resp)) {
                log_error("Transforming data response for source failed");
                inner_loop = false;
                goto cleanup_iteration;
            }

            if (!pumpnet_prinet_proxy_client_send_response(source_con, source_resp)) {
                log_error("Sending response to source failed");
                inner_loop = false;
                goto cleanup_iteration;
            }

        cleanup_iteration:
            if (source_req != NULL) {
                util_xfree((void**) &source_req);
            }

            if (pumpnet_data_req.bytes != NULL) {
                util_iobuf_free(&pumpnet_data_req);
            }

            if (pumpnet_data_resp.bytes != NULL) {
                util_iobuf_free(&pumpnet_data_resp);
            }

            if (source_resp != NULL) {
                util_xfree((void**) &source_resp);
            }
        } while (inner_loop);

        util_sock_tcp_close(source_con);
    }
}