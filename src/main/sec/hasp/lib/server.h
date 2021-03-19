#ifndef SEC_HASP_SERVER_H
#define SEC_HASP_SERVER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * Initialize the hasp server module. This just initializes the hasp
 * dongle emulator module
 *
 * @param key_data Pointer to loaded key data (attributes, key table)
 * @param len Length of the buffer
 */
void sec_hasp_server_init(const uint8_t *key_data, size_t len);

/**
 * Run the server. This call blocks until the server is shut down.
 * Recommended to run this in a separate thread.
 */
void sec_hasp_server_run(void);

/**
 * Check if the server is running
 */
bool sec_hasp_server_is_running(void);

/**
 * Signal a shut down to the server
 */
void sec_hasp_server_shutdown(void);

#endif
