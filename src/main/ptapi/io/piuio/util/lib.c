#include <stdlib.h>

#include "capnhook/hook/lib.h"

#include "ptapi/io/piuio/util/lib.h"

bool ptapi_io_piuio_util_lib_load(
    const char *path, struct ptapi_io_piuio_api *api)
{
  void *handle;

  handle = cnh_lib_load(path);

  if (handle == NULL) {
    return false;
  }

  api->ident = (ptapi_io_piuio_ident_t) cnh_lib_get_func_addr_handle(
      handle, "ptapi_io_piuio_ident");

  api->open = (ptapi_io_piuio_open_t) cnh_lib_get_func_addr_handle(
      handle, "ptapi_io_piuio_open");
  api->close = (ptapi_io_piuio_close_t) cnh_lib_get_func_addr_handle(
      handle, "ptapi_io_piuio_close");
  api->recv = (ptapi_io_piuio_recv_t) cnh_lib_get_func_addr_handle(
      handle, "ptapi_io_piuio_recv");
  api->send = (ptapi_io_piuio_send_t) cnh_lib_get_func_addr_handle(
      handle, "ptapi_io_piuio_send");
  api->get_input_pad =
      (ptapi_io_piuio_get_input_pad_t) cnh_lib_get_func_addr_handle(
          handle, "ptapi_io_piuio_get_input_pad");
  api->get_input_sys =
      (ptapi_io_piuio_get_input_sys_t) cnh_lib_get_func_addr_handle(
          handle, "ptapi_io_piuio_get_input_sys");
  api->set_output_pad =
      (ptapi_io_piuio_set_output_pad_t) cnh_lib_get_func_addr_handle(
          handle, "ptapi_io_piuio_set_output_pad");
  api->set_output_cab =
      (ptapi_io_piuio_set_output_cab_t) cnh_lib_get_func_addr_handle(
          handle, "ptapi_io_piuio_set_output_cab");

  return true;
}