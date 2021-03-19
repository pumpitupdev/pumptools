#define LOG_MODULE "patch-sigsegv"

#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "capnhook/hook/sig.h"

#include "util/log.h"

#include "sigsegv.h"

static bool patch_sigsevc_halt_on_segv;

static void patch_sigsegv_handler(int signal, __sighandler_t orig_handler)
{
  log_error("===================");
  log_error("!!!!! SIGSEGV !!!!!");

  void *array[128];
  size_t size;
  char **strings;

  size = (size_t) backtrace(array, sizeof(array));
  strings = backtrace_symbols(array, size);

  log_error("Backtrace (%d frames):", size);

  for (int i = 0; i < size; i++) {
    log_error("%s", strings[i]);
  }

  free(strings);

  if (patch_sigsevc_halt_on_segv) {
    log_error("Halting for debugger...");

    while (true) {
      sleep(1);
    }
  }

  exit(-1);
}

void patch_sigsegv_init(bool halt_on_segv)
{
  patch_sigsevc_halt_on_segv = halt_on_segv;

  cnh_sig_install(SIGSEGV, patch_sigsegv_handler);
  log_info("Initialized");
}