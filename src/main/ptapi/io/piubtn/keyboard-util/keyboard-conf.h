#ifndef PTAPI_IO_PIUBTN_KEYBOARD_UTIL_CONF_H
#define PTAPI_IO_PIUBTN_KEYBOARD_UTIL_CONF_H

#include <stdlib.h>

#include "io/util/joystick.h"

enum PTAPI_IO_PIUBTN_KEYBOARD_UTIL_CONF_INPUT {
    PTAPI_IO_PIUBTN_KEYBOARD_UTIL_CONF_INPUT_INVALID = 0,
    PTAPI_IO_PIUBTN_KEYBOARD_UTIL_CONF_INPUT_P1_LEFT = 1,
    PTAPI_IO_PIUBTN_KEYBOARD_UTIL_CONF_INPUT_P1_RIGHT = 2,
    PTAPI_IO_PIUBTN_KEYBOARD_UTIL_CONF_INPUT_P1_START = 3,
    PTAPI_IO_PIUBTN_KEYBOARD_UTIL_CONF_INPUT_P1_BACK = 4,
    PTAPI_IO_PIUBTN_KEYBOARD_UTIL_CONF_INPUT_P2_LEFT = 5,
    PTAPI_IO_PIUBTN_KEYBOARD_UTIL_CONF_INPUT_P2_RIGHT = 6,
    PTAPI_IO_PIUBTN_KEYBOARD_UTIL_CONF_INPUT_P2_START = 7,
    PTAPI_IO_PIUBTN_KEYBOARD_UTIL_CONF_INPUT_P2_BACK = 8,
    PTAPI_IO_PIUBTN_KEYBOARD_UTIL_CONF_INPUT_COUNT = 9,
};

extern const char* ptapi_io_piubtn_keyboard_util_conf_input_str[PTAPI_IO_PIUBTN_KEYBOARD_UTIL_CONF_INPUT_COUNT];

struct ptapi_io_piubtn_keyboard_util_conf {
    uint32_t button_map[PTAPI_IO_PIUBTN_KEYBOARD_UTIL_CONF_INPUT_COUNT];
};

void ptapi_io_piubtn_keyboard_util_conf_new(struct ptapi_io_piubtn_keyboard_util_conf** conf);

bool ptapi_io_piubtn_keyboard_util_conf_read_from_file(struct ptapi_io_piubtn_keyboard_util_conf** conf, const char* path);

bool ptapi_io_piubtn_keyboard_util_conf_write_to_file(struct ptapi_io_piubtn_keyboard_util_conf* conf, const char* path);

#endif
