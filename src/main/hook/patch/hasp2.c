#define LOG_MODULE "patch-hasp"

#include <pthread.h>

#include "sec/hasp/server.h"

#include "util/log.h"

static pthread_t patch_hasp_thread;

static void *patch_hasp_thread_run(void *args)
{
  log_info("Running hasp server thread");

  sec_hasp_server_run();

  log_info("Finished hasp server thread");

  return NULL;
}

void patch_hasp_init(const uint8_t *key_data, size_t len)
{
  sec_hasp_server_init(key_data, len);

  /* Run the daemon in another thread */
  pthread_create(&patch_hasp_thread, NULL, patch_hasp_thread_run, NULL);

  log_info("Initialized");
}

void patch_hasp_shutdown(void)
{
  log_info("Shutting down");

  sec_hasp_server_shutdown();

  pthread_join(patch_hasp_thread_run, NULL);
}