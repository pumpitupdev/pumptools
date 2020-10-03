#define LOG_MODULE "patch-microdog40"

#include <pthread.h>

#include "sec/microdog40/microdog40d.h"

#include "util/log.h"

static pthread_t patch_microdog40_thread;

static void* patch_microdog40_thread_run(void* args)
{
    log_info("Running daemon thread");

    sec_microdog40d_run();

    log_info("Finished daemon thread");

    return NULL;
}

void patch_microdog40_init(const uint8_t* key_data, size_t len)
{
    sec_microdog40d_init(key_data, len);

    /* Run the daemon in another thread */
    pthread_create(&patch_microdog40_thread, NULL,
        patch_microdog40_thread_run, NULL);

    log_info("Initialized");
}

void patch_microdog40_shutdown(void)
{
    log_info("Shutting down");

    sec_microdog40d_shutdown();

    pthread_join(patch_microdog40_thread, NULL);
}