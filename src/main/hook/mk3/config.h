/**
 * Patch the configuration file handling which uses libconfig. This hooks libconfig up to pumptools configuration
 * infrastructure.
 */
#ifndef MK3HOOK_CONFIG_H
#define MK3HOOK_CONFIG_H

#include "options.h"

/**
 * Initialize this module.
 *
 * @param options The options provided to the hook which are injected into libconfig config value reading.
 */
void mk3hook_config_init(const struct mk3hook_options* options);

#endif
