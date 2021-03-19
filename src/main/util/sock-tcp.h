//
// Created by on 9/16/18.
//

#ifndef UTIL_SOCK_TCP_H
#define UTIL_SOCK_TCP_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

bool util_sock_tcp_is_connectable(const char *ipv4, uint32_t timeout_ms);

bool util_sock_tcp_is_connectable2(
    const char *ipv4, uint16_t port, uint32_t timeout_ms);

bool util_sock_tcp_is_connectable3(
    uint32_t ipv4, uint16_t port, uint32_t timeout_ms);

int util_sock_tcp_connect(const char *ipv4);

int util_sock_tcp_connect2(const char *ipv4, uint16_t port);

int util_sock_tcp_connect3(uint32_t ipv4, uint16_t port);

ssize_t
util_sock_tcp_send(int handle, void *buffer, size_t size, uint32_t timeout_ms);

ssize_t
util_sock_tcp_recv(int handle, void *buffer, size_t size, uint32_t timeout_ms);

void util_sock_tcp_close(int handle);

#endif
