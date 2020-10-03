/**
 * Implementation of the piubtn API. This implements keyboard wrapper to play
 * (actually mainly test) any piu games supporting the piubtn IO
 */
#define LOG_MODULE "ptapi-io-piubtn-keyboard"

#include <assert.h>
#include <string.h>

#include "ptapi/io/piubtn.h"
#include "ptapi/io/x11-input-hook.h"

#include "ptapi/io/piubtn/keyboard-util/keyboard-conf.h"

#include "util/log.h"
#include "util/proc.h"
#include "util/str.h"

#define CONFIG_FILENAME "/piubtn-keyboard-conf.bin"
#define KEY_MAP_SIZE 0xFFFF

struct ptapi_io_piubtn_keyboard_util_conf* ptapi_io_piubtn_keyboard_conf;

static bool ptapi_io_piubtn_keyboard_buffer[KEY_MAP_SIZE];

static void _piubtn_emu_keyboard_dispatch_key_press(KeySym key);
static void _piubtn_emu_keyboard_dispatch_key_release(KeySym key);

static const struct ptapi_io_x11_input_hook_handler _piubtn_emu_keyboard_input_hook_handler = {
    .dispatch_key_press = _piubtn_emu_keyboard_dispatch_key_press,
    .dispatch_key_release = _piubtn_emu_keyboard_dispatch_key_release,
};

const char* ptapi_io_piubtn_ident(void)
{
    return "keyboard";
}

const struct ptapi_io_x11_input_hook_handler* ptapi_io_x11_input_handler_hook(void)
{
    return &_piubtn_emu_keyboard_input_hook_handler;
}

bool ptapi_io_piubtn_open()
{
    char path[PATH_MAX];
    char* config_path;

    // The game changes the working directory to the 'game' sub-folder. Therefore, ./my-config does not work here.
    if (!util_proc_get_folder_path_executable_no_ld_linux(path, sizeof(path))) {
        log_error("Getting executable folder path failed.");
        return false;
    }

    config_path = util_str_merge(path, CONFIG_FILENAME);

    if (!ptapi_io_piubtn_keyboard_util_conf_read_from_file(&ptapi_io_piubtn_keyboard_conf, config_path)) {
        log_error("Loading keyboard config file %s failed.", config_path);
        free(config_path);
        return false;
    }

    free(config_path);

    memset(ptapi_io_piubtn_keyboard_buffer, 0, sizeof(bool) * KEY_MAP_SIZE);

    return true;
}

void ptapi_io_piubtn_close(void)
{
    free(ptapi_io_piubtn_keyboard_conf);
}

bool ptapi_io_piubtn_recv(void)
{
    /* Nothing to poll */
    return true;
}

bool ptapi_io_piubtn_send(void)
{
    return true;
}

void ptapi_io_piubtn_get_input(uint8_t player, struct ptapi_io_piubtn_inputs* inputs)
{
    if (player == 0) {
        inputs->left = ptapi_io_piubtn_keyboard_buffer[
            ptapi_io_piubtn_keyboard_conf->button_map[PTAPI_IO_PIUBTN_KEYBOARD_UTIL_CONF_INPUT_P1_LEFT]];
        inputs->right = ptapi_io_piubtn_keyboard_buffer[
            ptapi_io_piubtn_keyboard_conf->button_map[PTAPI_IO_PIUBTN_KEYBOARD_UTIL_CONF_INPUT_P1_RIGHT]];
        inputs->start = ptapi_io_piubtn_keyboard_buffer[
            ptapi_io_piubtn_keyboard_conf->button_map[PTAPI_IO_PIUBTN_KEYBOARD_UTIL_CONF_INPUT_P1_START]];
        inputs->back = ptapi_io_piubtn_keyboard_buffer[
            ptapi_io_piubtn_keyboard_conf->button_map[PTAPI_IO_PIUBTN_KEYBOARD_UTIL_CONF_INPUT_P1_BACK]];
    } else {
        inputs->left = ptapi_io_piubtn_keyboard_buffer[
            ptapi_io_piubtn_keyboard_conf->button_map[PTAPI_IO_PIUBTN_KEYBOARD_UTIL_CONF_INPUT_P2_LEFT]];
        inputs->right = ptapi_io_piubtn_keyboard_buffer[
            ptapi_io_piubtn_keyboard_conf->button_map[PTAPI_IO_PIUBTN_KEYBOARD_UTIL_CONF_INPUT_P2_RIGHT]];
        inputs->start = ptapi_io_piubtn_keyboard_buffer[
            ptapi_io_piubtn_keyboard_conf->button_map[PTAPI_IO_PIUBTN_KEYBOARD_UTIL_CONF_INPUT_P2_START]];
        inputs->back = ptapi_io_piubtn_keyboard_buffer[
            ptapi_io_piubtn_keyboard_conf->button_map[PTAPI_IO_PIUBTN_KEYBOARD_UTIL_CONF_INPUT_P2_BACK]];
    }
}

void ptapi_io_piubtn_set_output(uint8_t player, const struct ptapi_io_piubtn_outputs* outputs)
{
    /* Not supported */
}

static void _piubtn_emu_keyboard_dispatch_key_press(KeySym key)
{
    if (key > 0 && key < KEY_MAP_SIZE) {
        ptapi_io_piubtn_keyboard_buffer[key] = true;
    }
}

static void _piubtn_emu_keyboard_dispatch_key_release(KeySym key)
{
    if (key > 0 && key < KEY_MAP_SIZE) {
        ptapi_io_piubtn_keyboard_buffer[key] = false;
    }
}