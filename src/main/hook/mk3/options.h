#ifndef MK3HOOK_OPTIONS_H
#define MK3HOOK_OPTIONS_H

#include <stdbool.h>
#include <stdint.h>

#include "util/log.h"

struct mk3hook_options {
  struct game {
    const char *settings;
    int32_t sync_offset;
    double sync_multiplier;
    double music_vol;
    double sfx_vol;
    bool fs_1st_2nd;
  } game;

  struct patch {
    struct gfx {
      bool windowed;
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
      bool debug_output;
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

bool mk3hook_options_init(
    int argc, char **argv, struct mk3hook_options *options);

#endif
