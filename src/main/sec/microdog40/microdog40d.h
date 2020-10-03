/**
 * Microdog 4.0 daemon module
 */
#ifndef SEC_MICRODOG40D_H
#define SEC_MICRODOG40D_H

#include <stdbool.h>
#include <stdint.h>

/**
 * Initialize the microdog (4.0) daemon module. This just initializes the
 * dongle emulator module
 *
 * @param key_data Pointer to loaded key data (dog attributes, key table)
 * @param len Length of the buffer
 */
void sec_microdog40d_init(const uint8_t* key_data, size_t len);

/**
 * Run the daemon. This call blocks until the daemon is shut down.
 * Recommended to run this in a separate thread.
 */
void sec_microdog40d_run(void);

/**
 * Check if the daemon is running
 */
bool sec_microdog40d_is_running(void);

/**
 * Signal a shut down to the daemon
 */
void sec_microdog40d_shutdown(void);

#endif