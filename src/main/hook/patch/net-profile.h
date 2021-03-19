/**
 * Patch module to redirect profile saving and loading to a pumpnet server (NX2
 * to Fiesta 2).
 */
#ifndef HOOK_PATCH_NET_PROFILE_H
#define HOOK_PATCH_NET_PROFILE_H

#include "pumpnet/lib/pumpnet.h"

/**
 * Initialize the module.
 *
 * @param game Game version to run this module on
 * @param pumpnet_server_addr Server address of pumpnet to connect to
 * @param machine_id Machine ID to use for authentication
 * @param cert_dir_path Path to a directory containing the client cert, client
 * key and CA cert bundle to enable https communication
 * @param verbose_debug_log Enable verbose debug log output, e.g. network
 * backend logging/traffic.
 */
void patch_net_profile_init(
    enum asset_game_version game,
    const char *pumpnet_server_addr,
    uint64_t machine_id,
    const char *cert_dir_path,
    bool verbose_debug_log);

/**
 * Shut down the module.
 */
void patch_net_profile_shutdown();

#endif
