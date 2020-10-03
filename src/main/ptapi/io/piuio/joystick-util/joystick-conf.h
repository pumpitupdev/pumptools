#ifndef PTAPI_IO_PIUIO_JOYSTICK_UTIL_CONF_H
#define PTAPI_IO_PIUIO_JOYSTICK_UTIL_CONF_H

#include <stdlib.h>

#include "io/util/joystick.h"

enum PTAPI_IO_PIUIO_JOYSTICK_UTIL_CONF_INPUT {
    PTAPI_IO_PIUIO_JOYSTICK_UTIL_CONF_INPUT_INVALID = 0,
    PTAPI_IO_PIUIO_JOYSTICK_UTIL_CONF_INPUT_P1_LU = 1,
    PTAPI_IO_PIUIO_JOYSTICK_UTIL_CONF_INPUT_P1_RU = 2,
    PTAPI_IO_PIUIO_JOYSTICK_UTIL_CONF_INPUT_P1_CN = 3,
    PTAPI_IO_PIUIO_JOYSTICK_UTIL_CONF_INPUT_P1_LD = 4,
    PTAPI_IO_PIUIO_JOYSTICK_UTIL_CONF_INPUT_P1_RD = 5,
    PTAPI_IO_PIUIO_JOYSTICK_UTIL_CONF_INPUT_P2_LU = 6,
    PTAPI_IO_PIUIO_JOYSTICK_UTIL_CONF_INPUT_P2_RU = 7,
    PTAPI_IO_PIUIO_JOYSTICK_UTIL_CONF_INPUT_P2_CN = 8,
    PTAPI_IO_PIUIO_JOYSTICK_UTIL_CONF_INPUT_P2_LD = 9,
    PTAPI_IO_PIUIO_JOYSTICK_UTIL_CONF_INPUT_P2_RD = 10,
    PTAPI_IO_PIUIO_JOYSTICK_UTIL_CONF_INPUT_SERVICE = 11,
    PTAPI_IO_PIUIO_JOYSTICK_UTIL_CONF_INPUT_TEST = 12,
    PTAPI_IO_PIUIO_JOYSTICK_UTIL_CONF_INPUT_CLEAR = 13,
    PTAPI_IO_PIUIO_JOYSTICK_UTIL_CONF_INPUT_COIN = 14,
    PTAPI_IO_PIUIO_JOYSTICK_UTIL_CONF_INPUT_COIN2 = 15,
    PTAPI_IO_PIUIO_JOYSTICK_UTIL_CONF_INPUT_COUNT = 16,
};

extern const char* ptapi_io_piuio_joystick_util_conf_input_str[PTAPI_IO_PIUIO_JOYSTICK_UTIL_CONF_INPUT_COUNT];

struct ptapi_io_piuio_joystick_util_conf_entry {
    char name[JOYSTICK_NAME_LEN];
    char dev_path[PATH_MAX];

    uint16_t button_map[PTAPI_IO_PIUIO_JOYSTICK_UTIL_CONF_INPUT_COUNT];
};

struct ptapi_io_piuio_joystick_util_conf {
    size_t num_entries;
    struct ptapi_io_piuio_joystick_util_conf_entry entries[];
};

void ptapi_io_piuio_joystick_util_conf_new(struct ptapi_io_piuio_joystick_util_conf** conf);

bool ptapi_io_piuio_joystick_util_conf_read_from_file(struct ptapi_io_piuio_joystick_util_conf** conf, const char* path);

bool ptapi_io_piuio_joystick_util_conf_write_to_file(struct ptapi_io_piuio_joystick_util_conf* conf, const char* path);

bool ptapi_io_piuio_joystick_util_conf_set_entry(struct ptapi_io_piuio_joystick_util_conf* conf,
        struct ptapi_io_piuio_joystick_util_conf_entry* entry);

void ptapi_io_piuio_joystick_util_conf_append_entry(struct ptapi_io_piuio_joystick_util_conf** conf,
        struct ptapi_io_piuio_joystick_util_conf_entry* entry);

bool ptapi_io_piuio_joystick_util_conf_find_entry(struct ptapi_io_piuio_joystick_util_conf* conf, const char* name,
        const char* dev_path, struct ptapi_io_piuio_joystick_util_conf_entry* entry);

#endif
