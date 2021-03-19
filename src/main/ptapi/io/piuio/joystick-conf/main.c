#include <stdio.h>
#include <unistd.h>

#include "io/util/joystick-util.h"
#include "io/util/joystick.h"

#include "ptapi/io/piuio/joystick-util/joystick-conf.h"

#include "util/log.h"
#include "util/mem.h"
#include "util/time.h"

#define MAX_JOYSTICKS 16
#define CONFIG_FILENAME "piuio-joystick-conf.bin"
#define SKIP_INPUT 0xFFFF

int kbhit()
{
  struct timeval tv = {0L, 0L};

  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(0, &fds);

  return select(1, &fds, NULL, NULL, &tv);
}

int getch()
{
  int r;
  unsigned char c;

  if ((r = read(0, &c, sizeof(c))) < 0) {
    return r;
  } else {
    return c;
  }
}

static uint16_t joystick_get_next_input(struct io_util_joystick *joystick)
{
  bool pressed;
  uint16_t button;

  pressed = false;
  button = 0;

  while (true) {
    if (!io_util_joystick_update(joystick)) {
      printf("Error updating joystick failed.\n");
      exit(-1);
    }

    // Allow skip-able input assignment via enter key on keyboard
    if (kbhit() > 0) {
      if (getch() == '\n') {
        return SKIP_INPUT;
      }
    }

    if (!pressed) {
      for (uint16_t j = 0; j < joystick->num_buttons; j++) {
        if (io_util_joystick_is_button_pressed(joystick, j)) {
          button = j;
          pressed = true;
          break;
        }
      }
    } else {
      // Wait for release of pressed input -> debouncing
      if (!io_util_joystick_is_button_pressed(joystick, button)) {
        break;
      }
    }

    util_time_sleep_ms(10);
  }

  return button;
}

static void menu_joystick_test()
{
  struct io_util_joystick_util_device_info dev_info[MAX_JOYSTICKS];
  size_t connected_joysticks;

  connected_joysticks = io_util_joystick_util_scan(dev_info, MAX_JOYSTICKS);

  printf("Connected joysticks: %d\n", connected_joysticks);

  for (size_t i = 0; i < connected_joysticks; i++) {
    printf(
        "%d: %s -> %s, buttons %d, axes %d\n",
        i,
        dev_info[i].name,
        dev_info[i].dev_path,
        dev_info[i].num_buttons,
        dev_info[i].num_axes);
  }

  size_t selected_device_idx;

  printf("Select device: ");
  scanf("%d", &selected_device_idx);

  if (selected_device_idx > connected_joysticks) {
    printf("Invalid device id %d, out of range\n", selected_device_idx);
    return;
  }

  struct io_util_joystick *joystick;

  if (!io_util_joystick_open(
          dev_info[selected_device_idx].dev_path, &joystick)) {
    printf(
        "Error opening device %s -> %s",
        dev_info[selected_device_idx].name,
        dev_info[selected_device_idx].dev_path);
    return;
  }

  printf("Pressed + released inputs:\n");

  while (true) {
    printf("%d\n", joystick_get_next_input(joystick));
  }

  io_util_joystick_close(joystick);
}

static void menu_joystick_map()
{
  struct ptapi_io_piuio_joystick_util_conf *config;
  struct ptapi_io_piuio_joystick_util_conf_entry config_entry;

  struct io_util_joystick_util_device_info dev_info[MAX_JOYSTICKS];
  size_t connected_joysticks;
  struct io_util_joystick *joystick;

  connected_joysticks = io_util_joystick_util_scan(dev_info, MAX_JOYSTICKS);

  printf("Connected joysticks: %d\n", connected_joysticks);

  for (size_t i = 0; i < connected_joysticks; i++) {
    printf(
        "%d: %s -> %s, buttons %d, axes %d\n",
        i,
        dev_info[i].name,
        dev_info[i].dev_path,
        dev_info[i].num_buttons,
        dev_info[i].num_axes);
  }

  size_t selected_device_idx;

  printf("Select device: ");
  scanf("%d", &selected_device_idx);

  if (selected_device_idx > connected_joysticks) {
    printf("Invalid device id %d, out of range\n", selected_device_idx);
    return;
  }

  if (!ptapi_io_piuio_joystick_util_conf_read_from_file(
          &config, CONFIG_FILENAME)) {
    printf("Reading existing config file failed, creating new\n");

    ptapi_io_piuio_joystick_util_conf_new(&config);

    memset(
        &config_entry,
        0,
        sizeof(struct ptapi_io_piuio_joystick_util_conf_entry));

    strcpy(config_entry.dev_path, dev_info[selected_device_idx].dev_path);
    strcpy(config_entry.name, dev_info[selected_device_idx].name);
  } else {
    if (!ptapi_io_piuio_joystick_util_conf_find_entry(
            config,
            dev_info[selected_device_idx].name,
            dev_info[selected_device_idx].dev_path,
            &config_entry)) {
      printf(
          "Could not find existing mapping entry in config for joystick, "
          "creating new one\n");

      memset(
          &config_entry,
          0,
          sizeof(struct ptapi_io_piuio_joystick_util_conf_entry));

      strcpy(config_entry.dev_path, dev_info[selected_device_idx].dev_path);
      strcpy(config_entry.name, dev_info[selected_device_idx].name);
    } else {
      printf(
          "Found existing mapping entry in config for joystick, overwriting\n");
    }
  }

  printf(
      "Configuring device %s -> %s\n",
      dev_info[selected_device_idx].name,
      dev_info[selected_device_idx].dev_path);

  if (!io_util_joystick_open(
          dev_info[selected_device_idx].dev_path, &joystick)) {
    printf(
        "Error opening device %s -> %s",
        dev_info[selected_device_idx].name,
        dev_info[selected_device_idx].dev_path);
    return;
  }

  for (uint32_t i = PTAPI_IO_PIUIO_JOYSTICK_UTIL_CONF_INPUT_P1_LU;
       i < PTAPI_IO_PIUIO_JOYSTICK_UTIL_CONF_INPUT_COUNT;
       i++) {
    printf(
        "Press button to assign to \"%s\" or Enter/Return to skip: ",
        ptapi_io_piuio_joystick_util_conf_input_str[i]);
    fflush(stdout);

    config_entry.button_map[i] = joystick_get_next_input(joystick);

    if (config_entry.button_map[i] == SKIP_INPUT) {
      printf("SKIP\n");
    } else {
      printf("%d\n", config_entry.button_map[i]);
    }
  }

  io_util_joystick_close(joystick);

  printf(
      "Configuration of device %s -> %s done\n",
      dev_info[selected_device_idx].name,
      dev_info[selected_device_idx].dev_path);

  if (!ptapi_io_piuio_joystick_util_conf_set_entry(config, &config_entry)) {
    ptapi_io_piuio_joystick_util_conf_append_entry(&config, &config_entry);
  }

  ptapi_io_piuio_joystick_util_conf_write_to_file(config, CONFIG_FILENAME);

  free(config);

  printf("Configuration file saved: %s\n", CONFIG_FILENAME);
}

int main(int argc, char **argv)
{
  bool main_loop;

  util_log_set_file("joystick-conf.log", false);
  util_log_set_level(LOG_LEVEL_ERROR);

  printf("ptapi PIUIO joystick button mapping config tool\n");

  main_loop = true;

  while (main_loop) {
    uint32_t item;

    printf("Main menu, please select:\n");
    printf("1 > Joystick input test\n");
    printf("2 > Configure joystick mappings\n");
    printf("3 > Exit\n");
    printf("Select number: ");

    scanf("%d", &item);

    switch (item) {
      case 1:
        menu_joystick_test();
        break;

      case 2:
        menu_joystick_map();
        break;

      case 3:
        main_loop = false;
        break;

      default:
        printf("Invalid item chosen: %d\n", item);
        break;
    }
  }

  return 0;
}