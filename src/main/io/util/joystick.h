#ifndef IO_UTIL_JOYSTICK_H
#define IO_UTIL_JOYSTICK_H

#include <linux/limits.h>
#include <stdbool.h>
#include <stdint.h>

#define JOYSTICK_NAME_LEN 128

struct io_util_joystick {
  char dev_path[PATH_MAX];
  int handle;
  uint16_t num_buttons;
  uint16_t num_axes;
  char name[JOYSTICK_NAME_LEN];
  bool *button_state;
};

bool io_util_joystick_open(const char *dev_path, struct io_util_joystick **ctx);

bool io_util_joystick_close(struct io_util_joystick *ctx);

bool io_util_joystick_update(struct io_util_joystick *ctx);

bool io_util_joystick_is_button_pressed(
    struct io_util_joystick *ctx, uint16_t button);

#endif
