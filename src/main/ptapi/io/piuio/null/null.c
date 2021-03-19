/**
 * Implementation of the piuio API. Null driver for testing and starting piu
 * without a real io
 */
#define LOG_MODULE "ptapi-io-piuio-null"

#include "ptapi/io/piuio.h"

const char *ptapi_io_piuio_ident(void)
{
  return "null";
}

bool ptapi_io_piuio_open(void)
{
  return true;
}

void ptapi_io_piuio_close(void)
{
}

bool ptapi_io_piuio_recv(void)
{
  return true;
}

bool ptapi_io_piuio_send(void)
{
  return true;
}

void ptapi_io_piuio_get_input_pad(
    uint8_t player,
    enum ptapi_io_piuio_sensor_group sensor_group,
    struct ptapi_io_piuio_pad_inputs *inputs)
{
}

void ptapi_io_piuio_get_input_sys(struct ptapi_io_piuio_sys_inputs *inputs)
{
}

void ptapi_io_piuio_set_output_pad(
    uint8_t player, const struct ptapi_io_piuio_pad_outputs *outputs)
{
}

void ptapi_io_piuio_set_output_cab(
    const struct ptapi_io_piuio_cab_outputs *outputs)
{
  /* Not supported */
}