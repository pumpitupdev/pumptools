#define LOG_MODULE "exchook-io"

#include <stdlib.h>

#include "ptapi/io/piuio.h"
#include "ptapi/io/piuio/util/lib.h"

#include "hook/exc/mempatch.h"

#include "util/log.h"
#include "util/patch.h"

/* Taken from ioarcade.hpp nx2 src dump */
#define GI_P1_1 0x00000001
#define GI_P1_3 0x00000002
#define GI_P1_5 0x00000004
#define GI_P1_7 0x00000008
#define GI_P1_9 0x00000010

#define GI_P2_1 0x00000100
#define GI_P2_3 0x00000200
#define GI_P2_5 0x00000400
#define GI_P2_7 0x00000800
#define GI_P2_9 0x00001000

#define GI_TEST 0x00010000
#define GI_SERVICE 0x00020000
#define GI_CLEAR 0x00040000

#define GI_COIN 0x00100000
#define GI_COIN2 0x00200000

struct exchook_io_sensores {
  uint32_t lu : 4;
  uint32_t ru : 4;
  uint32_t cn : 4;
  uint32_t ld : 4;
  uint32_t rd : 4;
  uint32_t padding : 12;
};

static const uint8_t exchook_io_sensor_mappings[] = {1, 2, 3, 0};

static struct ptapi_io_piuio_api exchook_piuio_api;

static uint16_t exchook_io_check(void *obj);
static int32_t exchook_io_light_all(void *obj, uint8_t on);
static int32_t exchook_io_light_neon(void *obj, uint8_t on);
static int32_t
exchook_io_light_halogan(void *obj, uint8_t on, uint16_t light_id);
static int32_t exchook_io_light_p1_ccfl(void *obj, uint8_t on, uint16_t cmd);
static int32_t exchook_io_light_p2_ccfl(void *obj, uint8_t on, uint16_t cmd);
static int32_t exchook_io_input(void *obj);
static int32_t exchook_io_coin_input(void *obj);
static int32_t exchook_io_coin_input_2(void *obj);
static int32_t exchook_io_coin_counter_2(void *obj);

static uint32_t *exchook_io_game_input_stat;
static uint32_t *exchook_io_game_input_down;
static uint32_t *exchook_io_game_input_up;

static uint32_t exchook_io_game_input_stat_prev;

static struct exchook_io_sensores exchook_io_sensores_p1;
static struct exchook_io_sensores exchook_io_sensores_p2;
static struct ptapi_io_piuio_pad_outputs exchook_io_out_pad_p1;
static struct ptapi_io_piuio_pad_outputs exchook_io_out_pad_p2;
static struct ptapi_io_piuio_pad_outputs exchook_io_out_pad_press_p1;
static struct ptapi_io_piuio_pad_outputs exchook_io_out_pad_press_p2;
static struct ptapi_io_piuio_cab_outputs exchook_io_out_cab;

static bool exchook_io_exit_on_service_test;

void exchook_io_init(
    const struct exchook_mempatch_table *patch_table,
    const char *piuio_lib_path,
    bool exit_on_service_test)
{
  if (!piuio_lib_path) {
    log_error(
        "NULL piuio (emu) lib path. Check that you have set-up and configured "
        "a piuio API implementation "
        "correctly.");
    return;
  }

  /* Load piuio library funcs */
  log_info("Loading piuio api implementation %s", piuio_lib_path);

  if (!ptapi_io_piuio_util_lib_load(piuio_lib_path, &exchook_piuio_api)) {
    log_error("Loading piuio API from file %s failed", piuio_lib_path);
    return;
  }

  exchook_io_exit_on_service_test = exit_on_service_test;

  log_info("Service + Test exit %d", exchook_io_exit_on_service_test);

  /* Hook MK5IO engine calls */
  /* Why so many functions you might ask? Because the MK5 IO code is a pile
     of shit created by a monkey */

  util_patch_function(patch_table->addr_io_check, exchook_io_check);
  util_patch_function(patch_table->addr_io_light_all, exchook_io_light_all);
  util_patch_function(patch_table->addr_io_light_neon, exchook_io_light_neon);
  util_patch_function(
      patch_table->addr_io_light_halogan, exchook_io_light_halogan);
  util_patch_function(
      patch_table->addr_io_light_p1_ccfl, exchook_io_light_p1_ccfl);
  util_patch_function(
      patch_table->addr_io_light_p2_ccfl, exchook_io_light_p2_ccfl);
  util_patch_function(patch_table->addr_io_input, exchook_io_input);
  util_patch_function(patch_table->addr_io_coin_input, exchook_io_coin_input);
  util_patch_function(
      patch_table->addr_io_coin_input_2, exchook_io_coin_input_2);
  util_patch_function(
      patch_table->addr_io_coin_counter_2, exchook_io_coin_counter_2);

  exchook_io_game_input_stat =
      ((uint32_t *) patch_table->addr_io_game_input_stat);
  exchook_io_game_input_down =
      ((uint32_t *) patch_table->addr_io_game_input_down);
  exchook_io_game_input_up = ((uint32_t *) patch_table->addr_io_game_input_up);

  /* there is an io init method but obviously requires another address to be
     hooked...just init our backend here */
  if (!exchook_piuio_api.open()) {
    log_error("Opening io failed");
    return;
  }

  log_info("Initialized");
}

void exchook_io_shutdown(void)
{
}

/* check? why can't you just call it get_fucking_inputs... */
static uint16_t exchook_io_check(void *obj)
{
  struct ptapi_io_piuio_pad_inputs p1_pad_in;
  struct ptapi_io_piuio_pad_inputs p2_pad_in;

  /* Sensor states are expected to be returned using the object */
  struct exchook_io_sensores *sensores_p1 = (struct exchook_io_sensores *) obj;
  struct exchook_io_sensores *sensores_p2 =
      (struct exchook_io_sensores *) (((uint32_t *) obj) + 1);

  /* execute polling here */

  /* might delay output for a frame but whatever */
  if (!exchook_piuio_api.send()) {
    log_error("Piuio send failed");
  }

  if (!exchook_piuio_api.recv()) {
    log_error("Piuio receive failed");
  }

  /* MK5 inputs state reset */
  exchook_io_sensores_p1.lu = 0;
  exchook_io_sensores_p1.ru = 0;
  exchook_io_sensores_p1.cn = 0;
  exchook_io_sensores_p1.ld = 0;
  exchook_io_sensores_p1.rd = 0;

  exchook_io_sensores_p2.lu = 0;
  exchook_io_sensores_p2.ru = 0;
  exchook_io_sensores_p2.cn = 0;
  exchook_io_sensores_p2.ld = 0;
  exchook_io_sensores_p2.rd = 0;

  // State reset
  memset(&p1_pad_in, 0, sizeof(struct ptapi_io_piuio_pad_inputs));
  memset(&p2_pad_in, 0, sizeof(struct ptapi_io_piuio_pad_inputs));

  /* cycle all sensores */
  for (uint8_t i = 0; i < 4; i++) {
    exchook_piuio_api.get_input_pad(0, i, &p1_pad_in);
    exchook_piuio_api.get_input_pad(1, i, &p2_pad_in);

    /* process inputs */

    /* Player 1 */
    if (p1_pad_in.lu) {
      exchook_io_sensores_p1.lu |= (1 << exchook_io_sensor_mappings[i]);
    }

    if (p1_pad_in.ru) {
      exchook_io_sensores_p1.ru |= (1 << exchook_io_sensor_mappings[i]);
    }

    if (p1_pad_in.cn) {
      exchook_io_sensores_p1.cn |= (1 << exchook_io_sensor_mappings[i]);
    }

    if (p1_pad_in.ld) {
      exchook_io_sensores_p1.ld |= (1 << exchook_io_sensor_mappings[i]);
    }

    if (p1_pad_in.rd) {
      exchook_io_sensores_p1.rd |= (1 << exchook_io_sensor_mappings[i]);
    }

    /* Player 2 */
    if (p2_pad_in.lu) {
      exchook_io_sensores_p2.lu |= (1 << exchook_io_sensor_mappings[i]);
    }

    if (p2_pad_in.ru) {
      exchook_io_sensores_p2.ru |= (1 << exchook_io_sensor_mappings[i]);
    }

    if (p2_pad_in.cn) {
      exchook_io_sensores_p2.cn |= (1 << exchook_io_sensor_mappings[i]);
    }

    if (p2_pad_in.ld) {
      exchook_io_sensores_p2.ld |= (1 << exchook_io_sensor_mappings[i]);
    }

    if (p2_pad_in.rd) {
      exchook_io_sensores_p2.rd |= (1 << exchook_io_sensor_mappings[i]);
    }
  }

  memcpy(sensores_p1, &exchook_io_sensores_p1, sizeof(exchook_io_sensores_p1));
  memcpy(sensores_p2, &exchook_io_sensores_p2, sizeof(exchook_io_sensores_p2));

  return 0;
}

static int32_t exchook_io_light_all(void *obj, uint8_t on)
{
  if (on) {
    memset(&exchook_io_out_pad_p1, 1, sizeof(exchook_io_out_pad_p1));
    memset(&exchook_io_out_pad_p2, 1, sizeof(exchook_io_out_pad_p2));
    memset(&exchook_io_out_cab, 1, sizeof(exchook_io_out_cab));
  } else {
    memset(&exchook_io_out_pad_p1, 0, sizeof(exchook_io_out_pad_p1));
    memset(&exchook_io_out_pad_p2, 0, sizeof(exchook_io_out_pad_p2));
    memset(&exchook_io_out_cab, 0, sizeof(exchook_io_out_cab));
  }

  exchook_piuio_api.set_output_pad(0, &exchook_io_out_pad_p1);
  exchook_piuio_api.set_output_pad(1, &exchook_io_out_pad_p2);
  exchook_piuio_api.set_output_cab(&exchook_io_out_cab);

  return 0;
}

static int32_t exchook_io_light_neon(void *obj, uint8_t on)
{
  if (on) {
    exchook_io_out_cab.bass = true;
  } else {
    exchook_io_out_cab.bass = false;
  }

  exchook_piuio_api.set_output_cab(&exchook_io_out_cab);

  return 0;
}

static int32_t
exchook_io_light_halogan(void *obj, uint8_t on, uint16_t light_id)
{
  if (light_id & 0x0400) {
    exchook_io_out_cab.halo_l1 = on;
  }

  if (light_id & 0x0200) {
    exchook_io_out_cab.halo_l2 = on;
  }

  if (light_id & 0x0100) {
    exchook_io_out_cab.halo_r1 = on;
  }

  if (light_id & 0x0080) {
    exchook_io_out_cab.halo_r2 = on;
  }

  exchook_piuio_api.set_output_cab(&exchook_io_out_cab);

  return 0;
}

static int32_t exchook_io_light_p1_ccfl(void *obj, uint8_t on, uint16_t cmd)
{
  if (cmd & 0x0004) {
    exchook_io_out_pad_p1.lu = on;
  }

  if (cmd & 0x0008) {
    exchook_io_out_pad_p1.ru = on;
  }

  if (cmd & 0x0010) {
    exchook_io_out_pad_p1.cn = on;
  }

  if (cmd & 0x0020) {
    exchook_io_out_pad_p1.ld = on;
  }

  if (cmd & 0x040) {
    exchook_io_out_pad_p1.rd = on;
  }

  return 0;
}

static int32_t exchook_io_light_p2_ccfl(void *obj, uint8_t on, uint16_t cmd)
{
  if (cmd & 0x0004) {
    exchook_io_out_pad_p2.lu = on;
  }

  if (cmd & 0x0008) {
    exchook_io_out_pad_p2.ru = on;
  }

  if (cmd & 0x0010) {
    exchook_io_out_pad_p2.cn = on;
  }

  if (cmd & 0x0020) {
    exchook_io_out_pad_p2.ld = on;
  }

  if (cmd & 0x040) {
    exchook_io_out_pad_p2.rd = on;
  }

  return 0;
}

static int32_t exchook_io_input(void *obj)
{
  uint32_t stat;
  uint32_t stat_down;
  uint32_t stat_up;

  struct ptapi_io_piuio_sys_inputs sys_in;

  memset(&sys_in, 0, sizeof(struct ptapi_io_piuio_sys_inputs));

  exchook_piuio_api.get_input_sys(&sys_in);

  /* For game loaders and stuff */
  if (exchook_io_exit_on_service_test) {
    if (sys_in.test && sys_in.service) {
      log_info("Exit on service + test enabled and hit, bye");
      exit(0);
    }
  }

  /* process final input states for the game as well as service, test, clear
     and coin which can't be handled in the check function */

  stat = 0;
  stat_down = 0;
  stat_up = 0;

  /* Player 1 */
  if (exchook_io_sensores_p1.lu) {
    stat |= GI_P1_7;
  }

  if (exchook_io_sensores_p1.ru) {
    stat |= GI_P1_9;
  }

  if (exchook_io_sensores_p1.cn) {
    stat |= GI_P1_5;
  }

  if (exchook_io_sensores_p1.ld) {
    stat |= GI_P1_1;
  }

  if (exchook_io_sensores_p1.rd) {
    stat |= GI_P1_3;
  }

  /* Player 2 */
  if (exchook_io_sensores_p2.lu) {
    stat |= GI_P2_7;
  }

  if (exchook_io_sensores_p2.ru) {
    stat |= GI_P2_9;
  }

  if (exchook_io_sensores_p2.cn) {
    stat |= GI_P2_5;
  }

  if (exchook_io_sensores_p2.ld) {
    stat |= GI_P2_1;
  }

  if (exchook_io_sensores_p2.rd) {
    stat |= GI_P2_3;
  }

  /* Sys */
  if (sys_in.test) {
    stat |= GI_TEST;
  }

  if (sys_in.service) {
    stat |= GI_SERVICE;
  }

  /* Taken from source but doesn't seem to work (on some versions?) */
  if (sys_in.clear) {
    stat |= GI_CLEAR;
  }

  if (sys_in.coin) {
    stat |= GI_COIN;
  }

  if (sys_in.coin2) {
    stat |= GI_COIN2;
  }

  /* iterate bits for up and down events */
  for (uint8_t i = 0; i < 32; i++) {

    if (!(exchook_io_game_input_stat_prev & (1 << i)) && (stat & (1 << i))) {
      /* New push down */
      stat_down |= (1 << i);
    } else if (
        (exchook_io_game_input_stat_prev & (1 << i)) && !(stat & (1 << i))) {
      /* New release */
      stat_up |= (1 << i);
    }
  }

  /* Extremely bad IO subsystem design */
  *exchook_io_game_input_stat |= stat;
  *exchook_io_game_input_down |= stat_down;
  *exchook_io_game_input_up = stat_up;

  /* swap for next iterateion */
  exchook_io_game_input_stat_prev = *exchook_io_game_input_stat;

  /* Welp, the game expects us to do output setting if a button is pressed
     here...fuck that mess. Merge (i.e. |= not =) with other states set
     by the game */

  memset(&exchook_io_out_pad_press_p1, 0, sizeof(exchook_io_out_pad_press_p1));
  memset(&exchook_io_out_pad_press_p2, 0, sizeof(exchook_io_out_pad_press_p2));

  /* Player 1 */
  exchook_io_out_pad_press_p1.lu |= exchook_io_sensores_p1.lu;
  exchook_io_out_pad_press_p1.ru |= exchook_io_sensores_p1.ru;
  exchook_io_out_pad_press_p1.cn |= exchook_io_sensores_p1.cn;
  exchook_io_out_pad_press_p1.ld |= exchook_io_sensores_p1.ld;
  exchook_io_out_pad_press_p1.rd |= exchook_io_sensores_p1.rd;

  /* Player 2 */
  exchook_io_out_pad_press_p2.lu |= exchook_io_sensores_p2.lu;
  exchook_io_out_pad_press_p2.ru |= exchook_io_sensores_p2.ru;
  exchook_io_out_pad_press_p2.cn |= exchook_io_sensores_p2.cn;
  exchook_io_out_pad_press_p2.ld |= exchook_io_sensores_p2.ld;
  exchook_io_out_pad_press_p2.rd |= exchook_io_sensores_p2.rd;

  /* merge */
  /* Player 1 */
  exchook_io_out_pad_press_p1.lu |= exchook_io_out_pad_p1.lu;
  exchook_io_out_pad_press_p1.ru |= exchook_io_out_pad_p1.ru;
  exchook_io_out_pad_press_p1.cn |= exchook_io_out_pad_p1.cn;
  exchook_io_out_pad_press_p1.ld |= exchook_io_out_pad_p1.ld;
  exchook_io_out_pad_press_p1.rd |= exchook_io_out_pad_p1.rd;

  /* Player 2 */
  exchook_io_out_pad_press_p2.lu |= exchook_io_out_pad_p2.lu;
  exchook_io_out_pad_press_p2.ru |= exchook_io_out_pad_p2.ru;
  exchook_io_out_pad_press_p2.cn |= exchook_io_out_pad_p2.cn;
  exchook_io_out_pad_press_p2.ld |= exchook_io_out_pad_p2.ld;
  exchook_io_out_pad_press_p2.rd |= exchook_io_out_pad_p2.rd;

  exchook_piuio_api.set_output_pad(0, &exchook_io_out_pad_press_p1);
  exchook_piuio_api.set_output_pad(1, &exchook_io_out_pad_press_p2);

  return 0;
}

static int32_t exchook_io_coin_input(void *obj)
{
  /* Stub */

  return 0;
}

static int32_t exchook_io_coin_input_2(void *obj)
{
  /* Stub */

  return 0;
}

static int32_t exchook_io_coin_counter_2(void *obj)
{
  /* Stub */

  return 0;
}