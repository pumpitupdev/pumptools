/**
 * Implementation of the piuio API. This implements keyboard wrapper to play
 * (actually mainly test) any piu games supporting a (mk6) usb piuio
 */
#define LOG_MODULE "ptapi-io-piuio-keyboard"

#include <string.h>

#include "ptapi/io/piuio.h"
#include "ptapi/io/x11-input-hook.h"

#include "ptapi/io/piuio/keyboard-util/keyboard-conf.h"

#include "util/log.h"
#include "util/proc.h"
#include "util/str.h"

#define CONFIG_FILENAME "/piuio-keyboard-conf.bin"
#define KEY_MAP_SIZE 0xFFFF

struct ptapi_io_piuio_keyboard_util_conf* ptapi_io_piuio_keyboard_conf;

static bool ptapi_io_piuio_keyboard_buffer[KEY_MAP_SIZE];

static void _piuio_emu_keyboard_dispatch_key_press(KeySym key);
static void _piuio_emu_keyboard_dispatch_key_release(KeySym key);

static const struct ptapi_io_x11_input_hook_handler _piuio_emu_keyboard_input_hook_handler = {
    .dispatch_key_press = _piuio_emu_keyboard_dispatch_key_press,
    .dispatch_key_release = _piuio_emu_keyboard_dispatch_key_release,
};

const char* ptapi_io_piuio_ident(void)
{
    return "keyboard";
}

const struct ptapi_io_x11_input_hook_handler* ptapi_io_x11_input_handler_hook(void)
{
    return &_piuio_emu_keyboard_input_hook_handler;
}

bool ptapi_io_piuio_open()
{
    char path[PATH_MAX];
    char* config_path;

    // The game changes the working directory to the 'game' sub-folder. Therefore, ./my-config does not work here.
    if (!util_proc_get_folder_path_executable_no_ld_linux(path, sizeof(path))) {
        log_error("Getting executable folder path failed.");
        return false;
    }

    config_path = util_str_merge(path, CONFIG_FILENAME);

    log_info("Loading configuration: ", config_path);

    if (!ptapi_io_piuio_keyboard_util_conf_read_from_file(&ptapi_io_piuio_keyboard_conf, config_path)) {
        log_error("Loading keyboard config file %s failed.", config_path);
        free(config_path);
        return false;
    }

    free(config_path);

    memset(ptapi_io_piuio_keyboard_buffer, 0, sizeof(bool) * KEY_MAP_SIZE);

    return true;
}

void ptapi_io_piuio_close(void)
{
    free(ptapi_io_piuio_keyboard_conf);
}

bool ptapi_io_piuio_recv(void)
{
    /* Nothing to poll */
    return true;
}

bool ptapi_io_piuio_send(void)
{
    return true;
}

void ptapi_io_piuio_get_input_pad(uint8_t player, enum ptapi_io_piuio_sensor_group sensor_group,
        struct ptapi_io_piuio_pad_inputs* inputs)
{
    if (player == 0) {
        inputs->lu = ptapi_io_piuio_keyboard_buffer[
            ptapi_io_piuio_keyboard_conf->button_map[PTAPI_IO_PIUIO_KEYBOARD_UTIL_CONF_INPUT_P1_LU]];
        inputs->ru = ptapi_io_piuio_keyboard_buffer[
            ptapi_io_piuio_keyboard_conf->button_map[PTAPI_IO_PIUIO_KEYBOARD_UTIL_CONF_INPUT_P1_RU]];
        inputs->cn = ptapi_io_piuio_keyboard_buffer[
            ptapi_io_piuio_keyboard_conf->button_map[PTAPI_IO_PIUIO_KEYBOARD_UTIL_CONF_INPUT_P1_CN]];
        inputs->ld = ptapi_io_piuio_keyboard_buffer[
            ptapi_io_piuio_keyboard_conf->button_map[PTAPI_IO_PIUIO_KEYBOARD_UTIL_CONF_INPUT_P1_LD]];
        inputs->rd = ptapi_io_piuio_keyboard_buffer[
            ptapi_io_piuio_keyboard_conf->button_map[PTAPI_IO_PIUIO_KEYBOARD_UTIL_CONF_INPUT_P1_RD]];
    } else {
        inputs->lu = ptapi_io_piuio_keyboard_buffer[
            ptapi_io_piuio_keyboard_conf->button_map[PTAPI_IO_PIUIO_KEYBOARD_UTIL_CONF_INPUT_P2_LU]];
        inputs->ru = ptapi_io_piuio_keyboard_buffer[
            ptapi_io_piuio_keyboard_conf->button_map[PTAPI_IO_PIUIO_KEYBOARD_UTIL_CONF_INPUT_P2_RU]];
        inputs->cn = ptapi_io_piuio_keyboard_buffer[
            ptapi_io_piuio_keyboard_conf->button_map[PTAPI_IO_PIUIO_KEYBOARD_UTIL_CONF_INPUT_P2_CN]];
        inputs->ld = ptapi_io_piuio_keyboard_buffer[
            ptapi_io_piuio_keyboard_conf->button_map[PTAPI_IO_PIUIO_KEYBOARD_UTIL_CONF_INPUT_P2_LD]];
        inputs->rd = ptapi_io_piuio_keyboard_buffer[
            ptapi_io_piuio_keyboard_conf->button_map[PTAPI_IO_PIUIO_KEYBOARD_UTIL_CONF_INPUT_P2_RD]];
    }
}

void ptapi_io_piuio_get_input_sys(struct ptapi_io_piuio_sys_inputs* inputs)
{
    inputs->test = ptapi_io_piuio_keyboard_buffer[
            ptapi_io_piuio_keyboard_conf->button_map[PTAPI_IO_PIUIO_KEYBOARD_UTIL_CONF_INPUT_TEST]];
    inputs->service = ptapi_io_piuio_keyboard_buffer[
            ptapi_io_piuio_keyboard_conf->button_map[PTAPI_IO_PIUIO_KEYBOARD_UTIL_CONF_INPUT_SERVICE]];
    inputs->clear = ptapi_io_piuio_keyboard_buffer[
            ptapi_io_piuio_keyboard_conf->button_map[PTAPI_IO_PIUIO_KEYBOARD_UTIL_CONF_INPUT_CLEAR]];
    inputs->coin = ptapi_io_piuio_keyboard_buffer[
            ptapi_io_piuio_keyboard_conf->button_map[PTAPI_IO_PIUIO_KEYBOARD_UTIL_CONF_INPUT_COIN]];
    inputs->coin2 = ptapi_io_piuio_keyboard_buffer[
            ptapi_io_piuio_keyboard_conf->button_map[PTAPI_IO_PIUIO_KEYBOARD_UTIL_CONF_INPUT_COIN2]];
}

void ptapi_io_piuio_set_output_pad(uint8_t player, const struct ptapi_io_piuio_pad_outputs* outputs)
{
    /* Not supported */
}

void ptapi_io_piuio_set_output_cab(const struct ptapi_io_piuio_cab_outputs* outputs)
{
    /* Not supported */
}

static void _piuio_emu_keyboard_dispatch_key_press(KeySym key)
{
    if (key > 0 && key < KEY_MAP_SIZE) {
        ptapi_io_piuio_keyboard_buffer[key] = true;
    }
}

static void _piuio_emu_keyboard_dispatch_key_release(KeySym key)
{
    if (key > 0 && key < KEY_MAP_SIZE) {
        ptapi_io_piuio_keyboard_buffer[key] = false;
    }
}