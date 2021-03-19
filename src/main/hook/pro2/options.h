#ifndef PRO2HOOK_OPTIONS_H
#define PRO2HOOK_OPTIONS_H

#include <stdbool.h>
#include <stdint.h>

#include "util/log.h"

struct pro2hook_options {
  struct game {
    const char *data;
  } game;

  struct patch {
    struct hook_mon {
      bool file;
      bool fs;
      bool io;
      bool open;
      bool usb;
    } hook_mon;

    struct piubtn {
      const char *api_lib;
    } piubtn;

    struct piuio {
      const char *api_lib;
      bool exit_test_serv;
    } piuio;

    struct x11_event_loop {
      const char *api_lib;
      const char *api_lib2;
    } x11_event_loop;
  } patch;

  struct log {
    const char *file;
    enum util_log_level level;
  } log;
};

bool pro2hook_options_init(
    int argc, char **argv, struct pro2hook_options *options);

#endif
