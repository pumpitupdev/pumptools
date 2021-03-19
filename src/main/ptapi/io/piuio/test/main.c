/**
 * Tool to test implementations of the piuio API
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ptapi/io/piuio.h"
#include "ptapi/io/piuio/util/lib.h"

#include "util/log.h"

static struct ptapi_io_piuio_api api;

int main(int argc, char **argv)
{
  struct ptapi_io_piuio_pad_inputs pad_in[2][PTAPI_IO_PIUIO_SENSOR_GROUP_NUM];
  struct ptapi_io_piuio_sys_inputs sys_in;
  struct ptapi_io_piuio_pad_outputs pad_out[2];
  struct ptapi_io_piuio_cab_outputs cab_out;

  util_log_set_file("piuio-test.log", false);
  util_log_set_level(LOG_LEVEL_ERROR);

  if (argc < 2) {
    printf("Usage: %s <piuio.so> [debug]\n", argv[0]);
    return -1;
  }

  for (int i = 0; i < argc; i++) {
    if (!strcmp(argv[i], "debug")) {
      util_log_set_level(LOG_LEVEL_DEBUG);
      break;
    }
  }

  if (!ptapi_io_piuio_util_lib_load(argv[1], &api)) {
    printf("Loading piuio lib %s failed\n", argv[1]);
    return -2;
  }

  if (!api.open()) {
    return -3;
  }

  memset(&pad_in, 0, sizeof(pad_in));
  memset(&sys_in, 0, sizeof(sys_in));
  memset(&pad_out, 0, sizeof(pad_out));
  memset(&cab_out, 0, sizeof(cab_out));

  printf("Enter main loop by pressing any key (on the keyboard)\n");
  getchar();

  while (true) {
    if (!api.recv()) {
      return -4;
    }

    for (uint8_t i = 0; i < 2; i++) {
      for (uint8_t j = 0; j < PTAPI_IO_PIUIO_SENSOR_GROUP_NUM; j++) {
        api.get_input_pad(i, j, &pad_in[i][j]);
      }
    }

    api.get_input_sys(&sys_in);

    if (sys_in.test && sys_in.service) {
      break;
    }

    system("clear");

    printf("Press test + service to exit\n");
    printf(
        "| Test %d   Service %d    Clear %d   Coin %d |\n"
        "|-------------------||-------------------|\n"
        "|  %d             %d  ||  %d             %d  |\n"
        "|%d   %d         %d   %d||%d   %d         %d   %d|\n"
        "|  %d             %d  ||  %d             %d  |\n"
        "|         %d         ||         %d         |\n"
        "|       %d   %d       ||       %d   %d       |\n"
        "|         %d         ||         %d         |\n"
        "|  %d             %d  ||  %d             %d  |\n"
        "|%d   %d         %d   %d||%d   %d         %d   %d|\n"
        "|  %d             %d  ||  %d             %d  |\n"
        "|-------------------||-------------------|\n",
        sys_in.test,
        sys_in.service,
        sys_in.clear,
        sys_in.coin,

        pad_in[0][PTAPI_IO_PIUIO_SENSOR_GROUP_UP].lu,
        pad_in[0][PTAPI_IO_PIUIO_SENSOR_GROUP_UP].ru,
        pad_in[1][PTAPI_IO_PIUIO_SENSOR_GROUP_UP].lu,
        pad_in[1][PTAPI_IO_PIUIO_SENSOR_GROUP_UP].ru,

        pad_in[0][PTAPI_IO_PIUIO_SENSOR_GROUP_LEFT].lu,
        pad_in[0][PTAPI_IO_PIUIO_SENSOR_GROUP_RIGHT].lu,
        pad_in[0][PTAPI_IO_PIUIO_SENSOR_GROUP_LEFT].ru,
        pad_in[0][PTAPI_IO_PIUIO_SENSOR_GROUP_RIGHT].ru,
        pad_in[1][PTAPI_IO_PIUIO_SENSOR_GROUP_LEFT].lu,
        pad_in[1][PTAPI_IO_PIUIO_SENSOR_GROUP_RIGHT].lu,
        pad_in[1][PTAPI_IO_PIUIO_SENSOR_GROUP_LEFT].ru,
        pad_in[1][PTAPI_IO_PIUIO_SENSOR_GROUP_RIGHT].ru,

        pad_in[0][PTAPI_IO_PIUIO_SENSOR_GROUP_DOWN].lu,
        pad_in[0][PTAPI_IO_PIUIO_SENSOR_GROUP_DOWN].ru,
        pad_in[1][PTAPI_IO_PIUIO_SENSOR_GROUP_DOWN].lu,
        pad_in[1][PTAPI_IO_PIUIO_SENSOR_GROUP_DOWN].ru,

        pad_in[0][PTAPI_IO_PIUIO_SENSOR_GROUP_UP].cn,
        pad_in[1][PTAPI_IO_PIUIO_SENSOR_GROUP_UP].cn,

        pad_in[0][PTAPI_IO_PIUIO_SENSOR_GROUP_LEFT].cn,
        pad_in[0][PTAPI_IO_PIUIO_SENSOR_GROUP_RIGHT].cn,
        pad_in[1][PTAPI_IO_PIUIO_SENSOR_GROUP_LEFT].cn,
        pad_in[1][PTAPI_IO_PIUIO_SENSOR_GROUP_RIGHT].cn,

        pad_in[0][PTAPI_IO_PIUIO_SENSOR_GROUP_DOWN].cn,
        pad_in[1][PTAPI_IO_PIUIO_SENSOR_GROUP_DOWN].cn,

        pad_in[0][PTAPI_IO_PIUIO_SENSOR_GROUP_UP].ld,
        pad_in[0][PTAPI_IO_PIUIO_SENSOR_GROUP_UP].rd,
        pad_in[1][PTAPI_IO_PIUIO_SENSOR_GROUP_UP].ld,
        pad_in[1][PTAPI_IO_PIUIO_SENSOR_GROUP_UP].rd,

        pad_in[0][PTAPI_IO_PIUIO_SENSOR_GROUP_LEFT].ld,
        pad_in[0][PTAPI_IO_PIUIO_SENSOR_GROUP_RIGHT].ld,
        pad_in[0][PTAPI_IO_PIUIO_SENSOR_GROUP_LEFT].rd,
        pad_in[0][PTAPI_IO_PIUIO_SENSOR_GROUP_RIGHT].rd,
        pad_in[1][PTAPI_IO_PIUIO_SENSOR_GROUP_LEFT].ld,
        pad_in[1][PTAPI_IO_PIUIO_SENSOR_GROUP_RIGHT].ld,
        pad_in[1][PTAPI_IO_PIUIO_SENSOR_GROUP_LEFT].rd,
        pad_in[1][PTAPI_IO_PIUIO_SENSOR_GROUP_RIGHT].rd,

        pad_in[0][PTAPI_IO_PIUIO_SENSOR_GROUP_DOWN].ld,
        pad_in[0][PTAPI_IO_PIUIO_SENSOR_GROUP_DOWN].rd,
        pad_in[1][PTAPI_IO_PIUIO_SENSOR_GROUP_DOWN].ld,
        pad_in[1][PTAPI_IO_PIUIO_SENSOR_GROUP_DOWN].rd);

    memset(&pad_out, 0, sizeof(pad_out));

    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < PTAPI_IO_PIUIO_SENSOR_GROUP_NUM; j++) {
        if (pad_in[i][j].lu) {
          pad_out[i].lu = true;
        }

        if (pad_in[i][j].ru) {
          pad_out[i].ru = true;
        }

        if (pad_in[i][j].cn) {
          pad_out[i].cn = true;
        }

        if (pad_in[i][j].ld) {
          pad_out[i].ld = true;
        }

        if (pad_in[i][j].rd) {
          pad_out[i].rd = true;
        }
      }

      api.set_output_pad(i, &pad_out[i]);
    }

    if (!api.send()) {
      return -5;
    }

    /* Avoid hardware banging */
    usleep(1000);
  }

  api.close();

  return 0;
}