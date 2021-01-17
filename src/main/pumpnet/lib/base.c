#define LOG_MODULE "pumpnet-base"

#include "pumpnet/lib/base.h"
#include "pumpnet/lib/http.h"

#include "util/log.h"
#include "util/rand.h"
#include "util/time.h"

uint64_t pumpnet_lib_base_generate_tracing_id()
{
    return util_rand_gen_64();
}

ssize_t pumpnet_lib_base_get_put(
        uint64_t trace_id,
        const char* endpoint,
        void* req_buffer,
        size_t req_size,
        void* resp_buffer,
        size_t resp_size,
        bool is_post,
        uint32_t retry_count_on_error,
        uint32_t retry_backoff_ms)
{
    uint32_t http_code;
    ssize_t recv_size;

    http_code = 0;
    recv_size = -1;

    for (int i = 0; i < retry_count_on_error; i++) {
        recv_size = pumpnet_lib_http_get_put(
            trace_id,
            endpoint,
            req_buffer,
            req_size,
            resp_buffer,
            resp_size,
            &http_code,
            is_post);

        if (http_code == 0 || http_code == HTTP_CODE_PRECONDITION_FAILED) {
            log_warn("[%llX][%s][%s] Failed, http code %d, retrying (%d)...",
                trace_id,
                is_post ? "post" : "get",
                endpoint,
                http_code,
                i);

           util_time_sleep_ms(retry_backoff_ms);
        } else {
            break;
        }
    }

    if (recv_size == -1) {
        log_error("[%llX][%s][%s] Failed", trace_id, is_post ? "post" : "get", endpoint);

        return -1;
    } else if (http_code != 200) {
        log_error("[%llX][%s][%s] Get non successful: %d", trace_id, is_post ? "post" : "get", endpoint, http_code);
        return -1;
    }

    return recv_size;
}