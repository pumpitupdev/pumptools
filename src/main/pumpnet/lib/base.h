#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t pumpnet_lib_base_generate_tracing_id();

ssize_t pumpnet_lib_base_get_put(
        uint64_t trace_id,
        const char* endpoint,
        void* req_buffer,
        size_t req_size,
        void* resp_buffer,
        size_t resp_size,
        bool is_post,
        uint32_t retry_count_on_error,
        uint32_t retry_backoff_ms);