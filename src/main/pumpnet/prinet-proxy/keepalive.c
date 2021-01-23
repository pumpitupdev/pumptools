#include <pthread.h>
#include <stdatomic.h>

#include "pumpnet/prinet-proxy/client.h"
#include "pumpnet/prinet-proxy/keepalive.h"
#include "pumpnet/prinet-proxy/packet.h"

#include "util/log.h"
#include "util/mem.h"
#include "util/time.h"

static const uint32_t _pumpnet_prinet_proxy_keepalive_thread_pause_ms = 1000;

static struct pumpnet_prinet_proxy_client_connection* _pumpnet_prinet_proxy_client_connection;
static uint32_t _pumpnet_prinet_proxy_keepalive_poll_ms;
static pthread_t _pumpnet_prinet_proxy_keepalive_thread;
static atomic_bool _pumpnet_prinet_proxy_keepalive_thread_run;

static void* _pumpnet_prinet_proxy_keepalive_proc(void* ctx)
{
    struct pumpnet_prinet_proxy_packet* keepalive_packet;
    uint64_t time_prev_ns;

    time_prev_ns = util_time_now_ns();

    log_debug("Keepalive thread started");

    while (_pumpnet_prinet_proxy_keepalive_thread_run) {
        uint64_t time_now_ns = util_time_now_ns();
        double time_delta_ms = util_time_delta_ms(time_prev_ns, time_now_ns);

        if (time_delta_ms >= _pumpnet_prinet_proxy_keepalive_poll_ms) {
            time_prev_ns = time_now_ns;

            if (pumpnet_prinet_proxy_client_connection_is_active(_pumpnet_prinet_proxy_client_connection)) {
                log_debug("Sending keepalive");

                keepalive_packet = pumpnet_prinet_proxy_packet_create_keepalive();

                size_t enc_data_len = sec_prinet_encrypt(
                    keepalive_packet->nounce,
                    sizeof(keepalive_packet->nounce),
                    keepalive_packet->data,
                    pumpnet_prinet_proxy_packet_get_data_len(keepalive_packet),
                    keepalive_packet->data);

                keepalive_packet->length = pumpnet_prinet_proxy_packet_get_header_len() + enc_data_len;

                if (!pumpnet_prinet_proxy_client_send_response(_pumpnet_prinet_proxy_client_connection, keepalive_packet)) {
                    log_error("Sending keepalive packet failed");
                }

                util_xfree((void**) &keepalive_packet);
            } else {
                log_debug("Skipping keepalive, no connection active");
            }
        }

        util_time_sleep_ms(_pumpnet_prinet_proxy_keepalive_thread_pause_ms);
    }

    log_debug("Keep alive thread finished");

    return NULL;
}

void pumpnet_prinet_proxy_keepalive_init(struct pumpnet_prinet_proxy_client_connection* connection, uint32_t poll_ms)
{
    _pumpnet_prinet_proxy_client_connection = connection;
    _pumpnet_prinet_proxy_keepalive_poll_ms = poll_ms;

    _pumpnet_prinet_proxy_keepalive_thread_run = true;

    int result = pthread_create(
        &_pumpnet_prinet_proxy_keepalive_thread,
        NULL,
        _pumpnet_prinet_proxy_keepalive_proc,
        NULL);

    if (result != 0) {
        log_error("Creating keepalive thread failed: %s", strerror(result));
    }

    log_info("Started keepalive thread with polling every %d ms", poll_ms);
}

void pumpnet_prinet_proxy_keepalive_shutdown()
{
    _pumpnet_prinet_proxy_keepalive_thread_run = false;

    pthread_join(_pumpnet_prinet_proxy_keepalive_thread, NULL);
}