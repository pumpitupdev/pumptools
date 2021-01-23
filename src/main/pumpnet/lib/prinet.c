#define LOG_MODULE "pumpnet-prinet"

#include <string.h>

#include "pumpnet/lib/base.h"
#include "pumpnet/lib/http.h"
#include "pumpnet/lib/prinet.h"
#include "pumpnet/lib/protocol-prinet.h"

#include "util/log.h"
#include "util/mem.h"
#include "util/rand.h"
#include "util/str.h"

// Client side on prime, this is around 136192 bytes, so 1 MB should be fairly save
#define RECV_BUFFER_SIZE 1024 * 1024
#define RETRY_COUNT_ON_ERROR 5
#define RETRY_BACKOFF_MS 1000

static enum asset_game_version _game;
static char* _server_addr;
static uint64_t _machine_id;

static char* _server_endpoint;

static char* _pumpnet_lib_prinet_get_endpoint(enum asset_game_version game, const char* server_addr)
{
    switch (game) {
        case ASSET_GAME_VERSION_PRIME:
            return util_str_merge(server_addr, PUMPNET_LIB_PRINET_ENDPOINT "/prime");

        default:
            log_die("Unsupported game version %d", game);
            return NULL;
    }
}

static struct pumpnet_lib_prinet_req* _pumpnet_lib_prinet_create_req(const void* req_buffer, size_t req_size)
{
    uint64_t trace_id;

    trace_id = pumpnet_lib_base_generate_tracing_id();

    struct pumpnet_lib_prinet_req* req;
    size_t req_struct_size = sizeof(struct pumpnet_lib_prinet_req) + req_size;

    req = util_xmalloc(req_struct_size);

    req->trace_id = trace_id;
    req->machine_id = _machine_id;
    req->size = req_size;

    memcpy(req->data, req_buffer, req_size);

    return req;
}

static struct pumpnet_lib_prinet_resp* _pumpnet_lib_prinet_create_resp(size_t max_resp_size)
{
    size_t resp_struct_size = sizeof(struct pumpnet_lib_prinet_resp) + max_resp_size;

    return util_xmalloc(resp_struct_size);
}

void pumpnet_lib_prinet_init(
        enum asset_game_version game,
        const char* server_addr,
        uint64_t machine_id,
        const char* cert_dir_path,
        bool verbose_debug_log)
{
    pumpnet_lib_http_init(cert_dir_path, RECV_BUFFER_SIZE, verbose_debug_log);

    _game = game;
    _server_addr = util_str_dup(server_addr);
    _machine_id = machine_id;

    _server_endpoint = _pumpnet_lib_prinet_get_endpoint(game, server_addr);

    log_info("Initialized pumpnet for game %d, serveraddr %s, main endpoint version %s",
        _game,
        _server_addr,
        PUMPNET_LIB_PRINET_ENDPOINT);
}

void pumpnet_lib_prinet_shutdown()
{
    free(_server_endpoint);

    pumpnet_lib_http_shutdown();

    log_info("Shut down");
}

ssize_t pumpnet_lib_prinet_msg(const void* req_buffer, size_t req_size, void* resp_buffer, size_t max_resp_size)
{
    log_assert(req_buffer);
    log_assert(resp_buffer);

    struct pumpnet_lib_prinet_req* req = _pumpnet_lib_prinet_create_req(req_buffer, req_size);
    struct pumpnet_lib_prinet_resp* resp = _pumpnet_lib_prinet_create_resp(max_resp_size);

    log_info("[%llX] Msg req, data size %d", req->trace_id, req->size);

    ssize_t recv_size = pumpnet_lib_base_get_put(
        req->trace_id,
        _server_endpoint,
        req,
        sizeof(struct pumpnet_lib_prinet_req) + req->size,
        resp,
        sizeof(struct pumpnet_lib_prinet_resp) + max_resp_size,
        true,
        RETRY_COUNT_ON_ERROR,
        RETRY_BACKOFF_MS);

    if (recv_size < 0) {
        free(req);
        free(resp);
        return -2;
    }

    // No response because uni-directional message
    if (recv_size == 0) {
        free(req);
        free(resp);
        return -1;
    }

    if (resp->size > max_resp_size) {
        log_error("Data received in response greater than max buffer size (%d > %d), "
          "memory corruption possible", resp->size, max_resp_size);

        free(req);
        free(resp);
        return -2;
    }

    memcpy(resp_buffer, resp->data, resp->size);
    size_t res_size = resp->size;

    free(req);
    free(resp);

    return res_size;
}