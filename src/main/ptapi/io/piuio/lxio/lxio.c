/**
 * Communicate with the Andamiro "lxio" or ATMEGAPUMP or PIU HID. 
 * This device hosts all data in two address, and interrupt in and out.
 * Device needs to be written then read in that order. Every read needs a preceding write.
 */
#define LOG_MODULE "ptapi-io-piuio-lxio"

#include <stdlib.h>
#include <string.h>

#include "io/lxio/defs.h"
#include "io/lxio/device.h"

#include "util/log.h"

#include "ptapi/io/piuio.h"
#include "ptapi/io/piubtn.h"

// Data going to/from lxio
uint8_t lxio_btn_state_buffer[LXIO_MSG_SIZE];
uint8_t lxio_light_state_buffer[LXIO_MSG_SIZE];

// data going to/from the ptapi caller
static struct ptapi_io_piuio_pad_inputs
    ptapi_piuio_pad_in[2][PTAPI_IO_PIUIO_SENSOR_GROUP_NUM];
static struct ptapi_io_piuio_sys_inputs ptapi_piuio_sys;
static struct ptapi_io_piuio_pad_outputs ptapi_piuio_pad_out[2];
static struct ptapi_io_piuio_cab_outputs ptapi_piuio_cab_out;
static struct ptapi_io_piubtn_outputs ptapi_piubtn_out[2];

const char *ptapi_io_piuio_ident(void)
{
  return "lxio";
}

bool ptapi_io_piuio_open(void)
{
  return lxio_drv_device_open();
}

void ptapi_io_piuio_close(void)
{
  lxio_drv_device_close();
}

void convert_output_to_lxio()
{
  memset(lxio_light_state_buffer, 0, sizeof(lxio_light_state_buffer));

  // light mapping follows the same format as the piuio
  // no sense in changing something if it's already set -andamiro probably.

  /* Pad lighting state. */
  for (uint8_t i = 0; i < 2; i++) {
    if (ptapi_piuio_pad_out[i].lu) {
      lxio_light_state_buffer[i * 2] |= (1 << 2);
    }

    if (ptapi_piuio_pad_out[i].ru) {
      lxio_light_state_buffer[i * 2] |= (1 << 3);
    }

    if (ptapi_piuio_pad_out[i].cn) {
      lxio_light_state_buffer[i * 2] |= (1 << 4);
    }

    if (ptapi_piuio_pad_out[i].ld) {
      lxio_light_state_buffer[i * 2] |= (1 << 5);
    }

    if (ptapi_piuio_pad_out[i].rd) {
      lxio_light_state_buffer[i * 2] |= (1 << 6);
    }
  }

  /* Neons/Bass */
  if (ptapi_piuio_cab_out.bass) {
    lxio_light_state_buffer[1] |= (1 << 2);
  }

  /* Halogens */

  if (ptapi_piuio_cab_out.halo_r1) {
    lxio_light_state_buffer[3] |= (1 << 0);
  }

  if (ptapi_piuio_cab_out.halo_l2) {
    lxio_light_state_buffer[3] |= (1 << 1);
  }

  if (ptapi_piuio_cab_out.halo_l1) {
    lxio_light_state_buffer[3] |= (1 << 2);
  }

  /* Menu Buttons */

  for (uint8_t i = 0; i < 2; i++) {
    //both UR and UL are mapped to "back"
    if (ptapi_piubtn_out[i].back) {
      lxio_light_state_buffer[i+4] |= (1 << 0);
      lxio_light_state_buffer[i+4] |= (1 << 1);
    }
    if (ptapi_piubtn_out[i].start) {
      lxio_light_state_buffer[i+4] |= (1 << 2);
    }
    if (ptapi_piubtn_out[i].left) {
      lxio_light_state_buffer[i+4] |= (1 << 3);
    }
    if (ptapi_piubtn_out[i].right) {
      lxio_light_state_buffer[i+4] |= (1 << 4);
    }
  }
}

static void convert_input_to_piuio()
{
  // LXIO sends all four sensor positions (the entire pad state 10*4) at once.
  // Each byte is a sensor position, so we need to take a look at the bit flags.
  // P1 is bytes 0->3 and P2 is 4->7 inclusive
  // and LXIO follows the same order as PTAPI/PIUIO in sensor order
  // that being lu, ru, cn, ld, rd (index: 0,1,2,3,4)
  for (uint8_t i = 0; i < 2; i++) {
    for (uint8_t j = 0; j < PTAPI_IO_PIUIO_SENSOR_GROUP_NUM; j++) {
      ptapi_piuio_pad_in[i][j].lu = lxio_btn_state_buffer[i * 4 + j] & (1 << 0);
      ptapi_piuio_pad_in[i][j].ru = lxio_btn_state_buffer[i * 4 + j] & (1 << 1);
      ptapi_piuio_pad_in[i][j].cn = lxio_btn_state_buffer[i * 4 + j] & (1 << 2);
      ptapi_piuio_pad_in[i][j].ld = lxio_btn_state_buffer[i * 4 + j] & (1 << 3);
      ptapi_piuio_pad_in[i][j].rd = lxio_btn_state_buffer[i * 4 + j] & (1 << 4);
    }
  }

  // cabinet buttons
  ptapi_piuio_sys.test = lxio_btn_state_buffer[8] & (1 << 1);
  ptapi_piuio_sys.service = lxio_btn_state_buffer[8] & (1 << 6);
  ptapi_piuio_sys.clear = lxio_btn_state_buffer[8] & (1 << 7);
  ptapi_piuio_sys.coin = lxio_btn_state_buffer[8] & (1 << 2);
  ptapi_piuio_sys.coin2 = lxio_btn_state_buffer[9] & (1 << 2);

}

bool ptapi_io_piuio_recv(void)
{
  // NOTE: The lxio *MUST* be written to in order to be read.
  // It cannot be read individually without a written payload. It will soft
  // lock. In order to ensure the full cycle, everything is taken care of here.

  // take the ptapi info and convert it to an lxio message.
  convert_output_to_lxio();

  if (!lxio_drv_device_write(lxio_light_state_buffer, LXIO_MSG_SIZE)) {
    return false;
  }

  if (!lxio_drv_device_read(lxio_btn_state_buffer, LXIO_MSG_SIZE)) {
    return false;
  }

  // Note: lxio contains an unsigned 16bit packet counter in the last two LSB
  // bytes, we can check that for errors....or not. libusb will tell us.

  // input from lxio is active low, so invert the recv data for active high
  // logic in ptapi.
  for (uint8_t j = 0; j < LXIO_MSG_SIZE; j++) {
    lxio_btn_state_buffer[j] ^= 0xFF;
  }

  // take the lxio message and convert it to ptapi.
  convert_input_to_piuio();

  return true;
}

bool ptapi_io_piuio_send(void)
{
  // taken care of in recv
  return true;
}

void ptapi_io_piuio_get_input_pad(
    uint8_t player,
    enum ptapi_io_piuio_sensor_group sensor_group,
    struct ptapi_io_piuio_pad_inputs *inputs)
{
  memcpy(
      inputs,
      &ptapi_piuio_pad_in[player][sensor_group],
      sizeof(struct ptapi_io_piuio_pad_inputs));
}

void ptapi_io_piuio_get_input_sys(struct ptapi_io_piuio_sys_inputs *inputs)
{
  memcpy(inputs, &ptapi_piuio_sys, sizeof(struct ptapi_io_piuio_sys_inputs));
}

void ptapi_io_piuio_set_output_pad(
    uint8_t player, const struct ptapi_io_piuio_pad_outputs *outputs)
{
  memcpy(
      &ptapi_piuio_pad_out[player],
      outputs,
      sizeof(struct ptapi_io_piuio_pad_outputs));
}

void ptapi_io_piuio_set_output_cab(
    const struct ptapi_io_piuio_cab_outputs *outputs)
{
  memcpy(
      &ptapi_piuio_cab_out, outputs, sizeof(struct ptapi_io_piuio_cab_outputs));
}

const char* ptapi_io_piubtn_ident(void)
{
  return "lxio";
}

bool ptapi_io_piubtn_open(void)
{
  //taken care of by piuio_open
  return true;
}

void ptapi_io_piubtn_close(void)
{
  //handled by piuio_close
}

bool ptapi_io_piubtn_recv(void)
{
  ptapi_io_piuio_recv();
  return true;
}

bool ptapi_io_piubtn_send(void)
{
  // taken care of in piuio_send
  return true;
}

void ptapi_io_piubtn_get_input(uint8_t player, struct ptapi_io_piubtn_inputs* inputs)
{
  // Note: There could be consistancy issues if the two IOs are threaded by seperate threads
  // Decided to take the risk of potentially lost button inputs. Testing has yielded
  // good results.

  // Set all inputs to off
  if (player == 0) {
      inputs->left = lxio_btn_state_buffer[10] & (1 << 3);
      inputs->right = lxio_btn_state_buffer[10] & (1 << 4);
      inputs->start = lxio_btn_state_buffer[10] & (1 << 2);

      //map both UR and UL to "back" as this is how kpump maps it.
      inputs->back = (lxio_btn_state_buffer[10] & (1 << 0)) |
                      (lxio_btn_state_buffer[10] & (1 << 1));
  } else {
      inputs->left = lxio_btn_state_buffer[11] & (1 << 3);
      inputs->right = lxio_btn_state_buffer[11] & (1 << 4);
      inputs->start = lxio_btn_state_buffer[11] & (1 << 2);

      //map both UR and UL to "back" as this is how kpump maps it.
      inputs->back = (lxio_btn_state_buffer[11] & (1 << 0)) |
                      (lxio_btn_state_buffer[11] & (1 << 1));
  }
}

void ptapi_io_piubtn_set_output(uint8_t player, const struct ptapi_io_piubtn_outputs* outputs)
{
  memcpy(&ptapi_piubtn_out[player], outputs, sizeof(struct ptapi_io_piubtn_outputs));
}
