#define LOG_MODULE "pumpnet-usbprofile"

#include <string.h>

#include "pumpnet/lib/base.h"
#include "pumpnet/lib/http.h"
#include "pumpnet/lib/protocol-usbprofile.h"
#include "pumpnet/lib/usbprofile.h"

#include "util/log.h"
#include "util/mem.h"
#include "util/rand.h"
#include "util/str.h"

#define RECV_BUFFER_SIZE 1024 * 1024
#define RETRY_COUNT_ON_ERROR 10
#define RETRY_BACKOFF_MS 2000

static enum asset_game_version pumpnet_lib_game;
static char* pumpnet_lib_server_addr;
static uint64_t pumpnet_lib_machine_id;

static char* pumpnet_lib_server_endpoint_save;
static char* pumpnet_lib_server_endpoint_rank;

static char* _pumpnet_lib_get_endpoint_save(enum asset_game_version game, const char* server_addr)
{
    switch (game) {
        case ASSET_GAME_VERSION_NX2:
            return util_str_merge(server_addr, USBPROFILE_ENDPOINT "/nx2/save");

        default:
            log_die("Unsupported game version %d", game);
            return NULL;
    }
}

static char* _pumpnet_lib_get_endpoint_rank(enum asset_game_version game, const char* server_addr)
{
    switch (game) {
        case ASSET_GAME_VERSION_NX2:
            return util_str_merge(server_addr, USBPROFILE_ENDPOINT "/nx2/rank");

        default:
            log_die("Unsupported game version %d", game);
            return NULL;
    }
}

static bool _pumpnet_lib_get_rank(uint64_t player_ref_id, void* buffer, size_t size)
{
    log_assert(buffer);

    struct pumpnet_lib_usbprofile_get_rank_req req;
    struct pumpnet_lib_usbprofile_get_rank_resp* resp;
    size_t resp_size;
    uint64_t trace_id;

    trace_id = pumpnet_lib_base_generate_tracing_id();

    req.trace_id = trace_id;
    req.machine_id = pumpnet_lib_machine_id;
    req.player_ref_id = player_ref_id;

    resp_size = sizeof(struct pumpnet_lib_usbprofile_get_rank_resp) + size;
    resp = util_xmalloc(resp_size);

    log_info("[%llX][%llX] Get rank", trace_id, player_ref_id);

    if (pumpnet_lib_base_get_put(
            trace_id,
            pumpnet_lib_server_endpoint_rank,
            &req,
            sizeof(req),
            resp,
            resp_size,
            false,
            RETRY_COUNT_ON_ERROR,
            RETRY_BACKOFF_MS) < 0) {
        free(resp);
        return false;
    }

    if (resp->size > size) {
        log_error("[%llX] Rank resp size greater than buffer size: %d > %d",
            trace_id,
            resp->size,
            size);
        free(resp);
        return false;
    }

    memcpy(buffer, resp->data, resp->size);

    free(resp);

    return true;
}

static bool _pumpnet_lib_get_save(uint64_t player_ref_id, void* buffer, size_t size)
{
    log_assert(buffer);

    struct pumpnet_lib_usbprofile_get_save_req req;
    struct pumpnet_lib_usbprofile_get_save_resp* resp;
    size_t resp_size;
    uint64_t trace_id;

    trace_id = pumpnet_lib_base_generate_tracing_id();

    req.trace_id = trace_id;
    req.machine_id = pumpnet_lib_machine_id;
    req.player_ref_id = player_ref_id;

    resp_size = sizeof(struct pumpnet_lib_usbprofile_get_save_resp) + size;
    resp = util_xmalloc(resp_size);

    log_info("[%llX][%llX] Get save", trace_id, player_ref_id);

    if (pumpnet_lib_base_get_put(
            trace_id,
            pumpnet_lib_server_endpoint_save,
            &req,
            sizeof(req),
            resp,
            resp_size,
            false,
            RETRY_COUNT_ON_ERROR,
            RETRY_BACKOFF_MS) < 0) {
        free(resp);
        return false;
    }

    if (resp->size > size) {
        log_error("[%llX] Save resp size greater than buffer size: %d > %d",
            trace_id,
            resp->size,
            size);
        free(resp);
        return false;
    }

    memcpy(buffer, resp->data, resp->size);

    free(resp);

    return true;
}

static bool _pumpnet_lib_put_rank(uint64_t player_ref_id, const void* buffer, size_t size)
{
    log_assert(buffer);

    struct pumpnet_lib_usbprofile_put_rank_req* req;
    struct pumpnet_lib_usbprofile_put_rank_resp resp;
    size_t req_size;
    uint64_t trace_id;

    trace_id = pumpnet_lib_base_generate_tracing_id();

    req_size = sizeof(struct pumpnet_lib_usbprofile_put_rank_req) + size;
    req = util_xmalloc(req_size);

    req->trace_id = trace_id;
    req->machine_id = pumpnet_lib_machine_id;
    req->player_ref_id = player_ref_id;
    req->size = size;
    memcpy(req->data, buffer, size);

    log_info("[%llX][%llX] Put rank", trace_id, player_ref_id);

    if (pumpnet_lib_base_get_put(
            trace_id,
            pumpnet_lib_server_endpoint_rank,
            req,
            req_size,
            &resp,
            sizeof(resp),
            true,
            RETRY_COUNT_ON_ERROR,
            RETRY_BACKOFF_MS) < 0) {
        free(req);
        return false;
    }

    free(req);

    return true;
}

static bool _pumpnet_lib_put_save(uint64_t player_ref_id, const void* buffer, size_t size)
{
    log_assert(buffer);

    struct pumpnet_lib_usbprofile_put_save_req* req;
    struct pumpnet_lib_usbprofile_put_save_resp resp;
    size_t req_size;
    uint64_t trace_id;

    trace_id = pumpnet_lib_base_generate_tracing_id();

    req_size = sizeof(struct pumpnet_lib_usbprofile_put_save_req) + size;
    req = util_xmalloc(req_size);

    req->trace_id = trace_id;
    req->machine_id = pumpnet_lib_machine_id;
    req->player_ref_id = player_ref_id;
    req->size = size;
    memcpy(req->data, buffer, size);

    log_info("[%llX][%llX] Put save", trace_id, player_ref_id);

    if (pumpnet_lib_base_get_put(
            trace_id,
            pumpnet_lib_server_endpoint_save,
            req,
            req_size,
            &resp,
            sizeof(resp),
            true,
            RETRY_COUNT_ON_ERROR,
            RETRY_BACKOFF_MS) < 0) {
        free(req);
        return false;
    }

    free(req);

    return true;
}

void pumpnet_lib_usbprofile_init(
        enum asset_game_version game,
        const char* server_addr,
        uint64_t machine_id,
        const char* cert_dir_path,
        bool verbose_debug_log)
{
    pumpnet_lib_http_init(cert_dir_path, RECV_BUFFER_SIZE, verbose_debug_log);

    pumpnet_lib_game = game;
    pumpnet_lib_server_addr = util_str_dup(server_addr);
    pumpnet_lib_machine_id = machine_id;

    pumpnet_lib_server_endpoint_save = _pumpnet_lib_get_endpoint_save(game, server_addr);
    pumpnet_lib_server_endpoint_rank = _pumpnet_lib_get_endpoint_rank(game, server_addr);

    log_info("Initialized pumpnet for game %d, serveraddr %s, main endpoint version %s",
        pumpnet_lib_game,
        pumpnet_lib_server_addr,
        USBPROFILE_ENDPOINT);
}

void pumpnet_lib_usbprofile_shutdown()
{
    free(pumpnet_lib_server_endpoint_save);
    free(pumpnet_lib_server_endpoint_rank);

    pumpnet_lib_http_shutdown();

    log_info("Shut down");
}

bool pumpnet_lib_usbprofile_get(enum pumpnet_lib_usbprofile_file_type file_type, uint64_t player_ref_id, void* buffer, size_t size)
{
    switch (file_type) {
        case PUMPNET_LIB_USBPROFILE_FILE_TYPE_SAVE:
            return _pumpnet_lib_get_save(player_ref_id, buffer, size);

        case PUMPNET_LIB_USBPROFILE_FILE_TYPE_RANK:
            return _pumpnet_lib_get_rank(player_ref_id, buffer, size);

        case PUMPNET_LIB_USBPROFILE_FILE_TYPE_COUNT:
        default:
            log_die_illegal_state();
            return false;
    }
}

bool pumpnet_lib_usbprofile_put(enum pumpnet_lib_usbprofile_file_type file_type, uint64_t player_ref_id, const void* buffer, size_t size)
{
    switch (file_type) {
        case PUMPNET_LIB_USBPROFILE_FILE_TYPE_SAVE:
            return _pumpnet_lib_put_save(player_ref_id, buffer, size);

        case PUMPNET_LIB_USBPROFILE_FILE_TYPE_RANK:
            return _pumpnet_lib_put_rank(player_ref_id, buffer, size);

        case PUMPNET_LIB_USBPROFILE_FILE_TYPE_COUNT:
        default:
            log_die_illegal_state();
            return false;
    }
}