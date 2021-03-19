/**
 * Redirect network traffic for game server (Prime and up).
 */
#ifndef PATCH_NETWORK_H
#define PATCH_NETWORK_H

#include <stdint.h>

/**
 * Initialize patch module
 */
void patch_network_init();

/**
 * Redirect a server address
 *
 * @param orig_ipv4 IPV4 address of the original server
 * @param new_ipv4 IPV4 address of the new server to redirect to
 * @param new_port Port of server to redirect to (-1 to keep old port)
 */
void patch_network_redirect_server_address(
    const char *orig_ipv4, const char *new_ipv4, uint16_t new_port);

#endif