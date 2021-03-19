#ifndef IO_UTIL_JOYSTICK_UTIL_H
#define IO_UTIL_JOYSTICK_UTIL_H

#include <linux/limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define JOYSTICK_NAME_LEN 128

struct io_util_joystick_util_device_info {
  char dev_path[PATH_MAX];
  uint16_t num_buttons;
  uint16_t num_axes;
  char name[JOYSTICK_NAME_LEN];
};

size_t io_util_joystick_util_scan(
    struct io_util_joystick_util_device_info *devices, size_t len);

#endif
