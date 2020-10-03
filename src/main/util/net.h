#ifndef UTIL_NET_H
#define UTIL_NET_H

#include <stdbool.h>
#include <stdint.h>

/**
 * Split an ipv4 string (e.g. 127.0.0.1:22 or just 127.0.0.1) into address and port
 *
 * @param str String to split
 * @param address Address part (must be free'd by caller)
 * @param port Port part (or -1 if it doesn't exist)
 * @return True if successful, false on format error
 */
bool util_net_split_ipv4_str(const char* str, char** address, uint16_t* port);

#endif
