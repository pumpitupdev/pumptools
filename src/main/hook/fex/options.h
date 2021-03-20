#ifndef FEXHOOK_OPTIONS_H
#define FEXHOOK_OPTIONS_H

#include <stdbool.h>
#include <stdint.h>

#include "util/log.h"

struct fexhook_options {
  struct game {
    const char *settings;
  } game;

  struct patch {
    struct gfx {
      uint8_t scaling_mode;
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

    struct net {
      const char *server;
      uint32_t machine_id;
    } net;

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

bool fexhook_options_init(
    int argc, char **argv, struct fexhook_options *options);

#endif
