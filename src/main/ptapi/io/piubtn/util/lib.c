#include <stdlib.h>

#include "capnhook/hook/lib.h"

#include "ptapi/io/piubtn/util/lib.h"

bool ptapi_io_piubtn_util_lib_load(const char* path, struct ptapi_io_piubtn_api* api)
{
    void* handle;

    handle = cnh_lib_load(path);

    if (handle == NULL) {
        return false;
    }

    api->ident = (ptapi_io_piubtn_ident_t) cnh_lib_get_func_addr_handle(
        handle, "ptapi_io_piubtn_ident");

    api->open = (ptapi_io_piubtn_open_t) cnh_lib_get_func_addr_handle(
        handle, "ptapi_io_piubtn_open");
    api->close = (ptapi_io_piubtn_close_t) cnh_lib_get_func_addr_handle(
        handle, "ptapi_io_piubtn_close");
    api->recv = (ptapi_io_piubtn_recv_t) cnh_lib_get_func_addr_handle(
        handle, "ptapi_io_piubtn_recv");
    api->send = (ptapi_io_piubtn_send_t) cnh_lib_get_func_addr_handle(
        handle, "ptapi_io_piubtn_send");
    api->get_input = (ptapi_io_piubtn_get_input_t)
        cnh_lib_get_func_addr_handle(handle, "ptapi_io_piubtn_get_input");
    api->set_output = (ptapi_io_piubtn_set_output_t)
        cnh_lib_get_func_addr_handle(handle, "ptapi_io_piubtn_set_output");

    return true;
}