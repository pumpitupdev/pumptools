#ifndef ZEROHOOK_OPTIONS_H
#define ZEROHOOK_OPTIONS_H

#include <stdbool.h>
#include <stdint.h>

#include "util/log.h"

struct zerohook_options {
  struct game {
    bool force_unlock;
    const char *settings;
  } game;

  struct patch {
    struct gfx {
      uint8_t scaling_mode;
      uint16_t frame_limit;
      uint8_t resizable_window;
      uint16_t screen_width;
      uint16_t screen_height;
    } gfx;

    struct hook_mon {
      bool file;
      bool fs;
      bool io;
      bool open;
      bool usb;
    } hook_mon;

    struct main_loop {
      bool disable_built_in_inputs;
      const char *x11_input_handler_api_lib;
    } main_loop;

    struct piuio {
      const char *api_lib;
      bool exit_test_serv;
    } piuio;

    struct sound {
      const char *device;
    } sound;

    struct sigsegv {
      bool halt_on_segv;
    } sigsegv;
  } patch;

  struct log {
    const char *file;
    enum util_log_level level;
  } log;
};

bool zerohook_options_init(
    int argc, char **argv, struct zerohook_options *options);

#endif
