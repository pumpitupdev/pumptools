/**
 * Blacklist calls to offical update and game server (Prime and up).
 */
#ifndef PATCH_BLACKLIST_URL_H
#define PATCH_BLACKLIST_URL_H

/**
 * Initialize patch module
 */
void patch_blacklist_url_init(void);

/**
 * Add an url to block to the blacklist
 *
 * @param url Url to block
 */
void patch_blacklist_url_add(const char *url);

#endif