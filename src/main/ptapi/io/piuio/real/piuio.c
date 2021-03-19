/**
 * Implementation of the piuio API. This implements a piuio usb driver to
 * communicate with real piuio hardware and also shares a common interface with
 * other "piuio devices" (e.g. keyboard, custom IOs)
 */
#define LOG_MODULE "ptapi-io-piuio-null"

#include <stdlib.h>
#include <string.h>

#include "io/piuio/defs.h"
#include "io/piuio/device.h"

#include "ptapi/io/piuio.h"

static uint8_t piuio_drv_piuio_in_buffer[PIUIO_DRV_BUFFER_SIZE];
static uint8_t piuio_drv_piuio_out_buffer[PIUIO_DRV_BUFFER_SIZE];

static struct ptapi_io_piuio_pad_inputs
    piuio_drv_piuio_pad_in[2][PTAPI_IO_PIUIO_SENSOR_GROUP_NUM];
static struct ptapi_io_piuio_sys_inputs piuio_drv_piuio_sys;
static struct ptapi_io_piuio_pad_outputs piuio_drv_piuio_pad_out[2];
static struct ptapi_io_piuio_cab_outputs piuio_drv_piuio_cab_out;

static void piuio_drv_piuio_copy_outputs(uint8_t sensor_group)
{
  memset(piuio_drv_piuio_out_buffer, 0, sizeof(piuio_drv_piuio_out_buffer));

  /* 00 = sensor 1 (right), 01 = sensor 2 (left), 10 = sensor 3 (down),
       11 = sensor 4 (up) */
  /* Pad 1 sensor group selector */
  piuio_drv_piuio_out_buffer[0] |= sensor_group;
  /* Pad 2 sensor group selector */
  piuio_drv_piuio_out_buffer[2] |= sensor_group;

  /* Set pad lights */
  for (uint8_t i = 0; i < 2; i++) {
    if (piuio_drv_piuio_pad_out[i].lu) {
      piuio_drv_piuio_out_buffer[i * 2] |= (1 << 2);
    }

    if (piuio_drv_piuio_pad_out[i].ru) {
      piuio_drv_piuio_out_buffer[i * 2] |= (1 << 3);
    }

    if (piuio_drv_piuio_pad_out[i].cn) {
      piuio_drv_piuio_out_buffer[i * 2] |= (1 << 4);
    }

    if (piuio_drv_piuio_pad_out[i].ld) {
      piuio_drv_piuio_out_buffer[i * 2] |= (1 << 5);
    }

    if (piuio_drv_piuio_pad_out[i].rd) {
      piuio_drv_piuio_out_buffer[i * 2] |= (1 << 6);
    }
  }

  /* Neons/Bass */
  if (piuio_drv_piuio_cab_out.bass) {
    piuio_drv_piuio_out_buffer[1] |= (1 << 2);
  }

  /* Halogens */
  if (piuio_drv_piuio_cab_out.halo_r2) {
    piuio_drv_piuio_out_buffer[2] |= (1 << 7);
  }

  if (piuio_drv_piuio_cab_out.halo_r1) {
    piuio_drv_piuio_out_buffer[3] |= (1 << 0);
  }

  if (piuio_drv_piuio_cab_out.halo_l2) {
    piuio_drv_piuio_out_buffer[3] |= (1 << 1);
  }

  if (piuio_drv_piuio_cab_out.halo_l1) {
    piuio_drv_piuio_out_buffer[3] |= (1 << 2);
  }

  // // set coin counter 1
  // piuio_drv_piuio_out_buffer[3] |= ((m_outputStates->m_counter & 0x01) << 4);

  // // set coin counter 2 (front usb enable)
  // piuio_drv_piuio_out_buffer[1] |= ((m_outputStates->m_counter & 0x02) << 3);
}

static void piuio_drv_piuio_copy_inputs(uint8_t sensor_group)
{
  /* Pad */
  for (uint8_t i = 0; i < 2; i++) {
    piuio_drv_piuio_pad_in[i][sensor_group].lu =
        piuio_drv_piuio_in_buffer[i * 2] & (1 << 0);
    piuio_drv_piuio_pad_in[i][sensor_group].ru =
        piuio_drv_piuio_in_buffer[i * 2] & (1 << 1);
    piuio_drv_piuio_pad_in[i][sensor_group].cn =
        piuio_drv_piuio_in_buffer[i * 2] & (1 << 2);
    piuio_drv_piuio_pad_in[i][sensor_group].ld =
        piuio_drv_piuio_in_buffer[i * 2] & (1 << 3);
    piuio_drv_piuio_pad_in[i][sensor_group].rd =
        piuio_drv_piuio_in_buffer[i * 2] & (1 << 4);
  }

  /* Sys */
  piuio_drv_piuio_sys.test = piuio_drv_piuio_in_buffer[1] & (1 << 1);
  piuio_drv_piuio_sys.service = piuio_drv_piuio_in_buffer[1] & (1 << 6);
  piuio_drv_piuio_sys.clear = piuio_drv_piuio_in_buffer[1] & (1 << 7);
  piuio_drv_piuio_sys.coin = piuio_drv_piuio_in_buffer[1] & (1 << 2);

  memset(piuio_drv_piuio_in_buffer, 0, sizeof(piuio_drv_piuio_in_buffer));
}

const char *ptapi_io_piuio_ident(void)
{
  return "piuio";
}

bool ptapi_io_piuio_open(void)
{
  return piuio_drv_device_open();
}

void ptapi_io_piuio_close(void)
{
  piuio_drv_device_close();
}

bool ptapi_io_piuio_recv(void)
{
  /* cycle all four sensor groups */
  for (uint8_t i = 0; i < PTAPI_IO_PIUIO_SENSOR_GROUP_NUM; i++) {
    piuio_drv_piuio_copy_outputs(i);

    if (!piuio_drv_device_write(
            piuio_drv_piuio_out_buffer, sizeof(piuio_drv_piuio_out_buffer))) {
      return false;
    }

    if (!piuio_drv_device_read(
            piuio_drv_piuio_in_buffer, sizeof(piuio_drv_piuio_in_buffer))) {
      return false;
    }

    /* Invert pull ups */
    for (uint8_t j = 0; j < 4; j++) {
      piuio_drv_piuio_in_buffer[j] ^= 0xFF;
    }

    piuio_drv_piuio_copy_inputs(i);
  }

  return true;
}

bool ptapi_io_piuio_send(void)
{
  /* Already done in recv */
  return true;
}

void ptapi_io_piuio_get_input_pad(
    uint8_t player,
    enum ptapi_io_piuio_sensor_group sensor_group,
    struct ptapi_io_piuio_pad_inputs *inputs)
{
  memcpy(
      inputs,
      &piuio_drv_piuio_pad_in[player][sensor_group],
      sizeof(struct ptapi_io_piuio_pad_inputs));
}

void ptapi_io_piuio_get_input_sys(struct ptapi_io_piuio_sys_inputs *inputs)
{
  memcpy(
      inputs, &piuio_drv_piuio_sys, sizeof(struct ptapi_io_piuio_sys_inputs));
}

void ptapi_io_piuio_set_output_pad(
    uint8_t player, const struct ptapi_io_piuio_pad_outputs *outputs)
{
  memcpy(
      &piuio_drv_piuio_pad_out[player],
      outputs,
      sizeof(struct ptapi_io_piuio_pad_outputs));
}

void ptapi_io_piuio_set_output_cab(
    const struct ptapi_io_piuio_cab_outputs *outputs)
{
  memcpy(
      &piuio_drv_piuio_cab_out,
      outputs,
      sizeof(struct ptapi_io_piuio_cab_outputs));
}