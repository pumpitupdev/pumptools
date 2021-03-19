#ifndef NX2HOOK_OPTIONS_H
#define NX2HOOK_OPTIONS_H

#include <stdbool.h>
#include <stdint.h>

#include "util/log.h"

struct nx2hook_options {
  struct game {
    const char *settings;
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

    struct net {
      const char *server;
      uint64_t machine_id;
      bool verbose_log_output;
      const char *cert_dir_path;
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

bool nx2hook_options_init(
    int argc, char **argv, struct nx2hook_options *options);

#endif
