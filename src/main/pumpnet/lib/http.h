#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define HTTP_CODE_OK 200
#define HTTP_CODE_PRECONDITION_FAILED 412

void pumpnet_lib_http_init(const char *cert_dir_path, bool verbose_debug_log);

void pumpnet_lib_http_shutdown();

bool pumpnet_lib_http_get_put(
    uint64_t trace_id,
    const char *address,
    void *send_data,
    size_t send_size,
    void *recv_data,
    size_t recv_size,
    uint32_t *http_code,
    bool is_post);