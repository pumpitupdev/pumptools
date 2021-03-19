#ifndef X2HOOK_OPTIONS_H
#define X2HOOK_OPTIONS_H

#include <stdbool.h>
#include <stdint.h>

#include "util/log.h"

struct x2hook_options {
  struct game {
    bool force_unlock;
    const char *settings;
  } game;

  struct patch {
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

bool x2hook_options_init(int argc, char **argv, struct x2hook_options *options);

#endif
