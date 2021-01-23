#define LOG_MODULE "sock-tcp"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "util/net.h"
#include "util/log.h"
#include "util/sock-tcp.h"

const int INVALID_SOCK_HANDLE = -1;

bool util_sock_tcp_is_connectable(const char* ipv4, uint32_t timeout_ms)
{
    char* address;
    uint16_t port;

    util_net_split_ipv4_str(ipv4, &address, &port);

    bool res = util_sock_tcp_is_connectable2(address, port, timeout_ms);

    free(address);
    return res;
}

bool util_sock_tcp_is_connectable2(const char* ipv4, uint16_t port, uint32_t timeout_ms)
{
    struct sockaddr_in sa;

    // store this IP address in sa:
    inet_pton(AF_INET, ipv4, &(sa.sin_addr));

    return util_sock_tcp_is_connectable3(htonl(sa.sin_addr.s_addr), port, timeout_ms);
}

bool util_sock_tcp_is_connectable3(uint32_t ipv4, uint16_t port, uint32_t timeout_ms)
{
    int handle;
    struct sockaddr_in addr;
    fd_set fdset;
    struct timeval tv;
    bool res;

    handle = socket(AF_INET, SOCK_STREAM, 0);

    if (handle == -1) {
        log_error("Creating socket (%X, %d) failed: %s", ipv4, port, strerror(errno));
        return false;
    }

    fcntl(handle, F_SETFL, O_NONBLOCK);

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(ipv4);

    connect(handle, (const struct sockaddr*) &addr, sizeof(addr));

    FD_ZERO(&fdset);
    FD_SET(handle, &fdset);
    tv.tv_sec = (__time_t) (timeout_ms / 1000);
    tv.tv_usec = (__suseconds_t) ((timeout_ms % 1000) * 1000);

    if (select(handle + 1, NULL, &fdset, NULL, &tv) == 1) {
        int so_error;
        socklen_t len = sizeof(so_error);

        getsockopt(handle, SOL_SOCKET, SO_ERROR, &so_error, &len);

        if (so_error == 0) {
            res = true;
        } else {
            res = false;
        }
    } else {
        res = false;
    }

    close(handle);

    return res;
}

int util_sock_tcp_open_bind_listen(const char* ipv4, uint32_t num_connections)
{
    char* address;
    uint16_t port;

    util_net_split_ipv4_str(ipv4, &address, &port);

    int res = util_sock_tcp_open_bind_listen2(address, port, num_connections);

    free(address);
    return res;
}

int util_sock_tcp_open_bind_listen2(const char* ipv4, uint16_t port, uint32_t num_connections)
{
    struct sockaddr_in sa;

    // store this IP address in sa:
    inet_pton(AF_INET, ipv4, &(sa.sin_addr));

    return util_sock_tcp_open_bind_listen3(htonl(sa.sin_addr.s_addr), port, num_connections);
}

int util_sock_tcp_open_bind_listen3(uint32_t ipv4, uint16_t port, uint32_t num_connections)
{
    int handle;
    struct sockaddr_in addr;

    handle = socket(AF_INET, SOCK_STREAM, 0);

    if (handle == -1) {
        log_error("Creating socket (%X, %d) failed: %s", ipv4, port, strerror(errno));
        return -1;
    }

    // Allow re-using address/port even when considered already bound. The kernel might hold to the port
    // a little longer, up to a minute, even after a clean shutdown of the application and closing the
    // socket. This avoids having to wait and quick application restart
    setsockopt(handle, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(ipv4);

    if (bind(handle, (const struct sockaddr*) &addr, sizeof(addr)) == -1) {
        close(handle);
        log_error("Binding to %X, %d failed: %s", ipv4, port, strerror(errno));
        return -1;
    }

    if (listen(handle, num_connections) == -1) {
        close(handle);
        log_error("Listening to %d connections on %X, %d failed: %s", num_connections, ipv4, port, strerror(errno));
        return -1;
    }

    log_debug("Socket opened (%X), bound and listening on %X, port %d", handle, ipv4, port);

    return handle;
}

int util_sock_tcp_wait_and_accept_remote_connection(int handle)
{
    int con_handle = accept(handle, (struct sockaddr*) NULL, NULL);

    if (con_handle == -1) {
        log_error("Accepting connection of socket (%X) failed: %s", handle, strerror(errno));
        return -1;
    }

    return con_handle;
}

int util_sock_tcp_connect(const char* ipv4)
{
    char* address;
    uint16_t port;

    util_net_split_ipv4_str(ipv4, &address, &port);

    int res = util_sock_tcp_connect2(address, port);

    free(address);
    return res;
}

int util_sock_tcp_connect2(const char* ipv4, uint16_t port)
{
    struct sockaddr_in sa;

    // store this IP address in sa:
    inet_pton(AF_INET, ipv4, &(sa.sin_addr));

    return util_sock_tcp_connect3(htonl(sa.sin_addr.s_addr), port);
}

int util_sock_tcp_connect3(uint32_t ipv4, uint16_t port)
{
    int handle;
    struct sockaddr_in addr;

    handle = socket(AF_INET, SOCK_STREAM, 0);

    if (handle == -1) {
        log_error("Creating socket (%X, %d) failed: %s", ipv4, port, strerror(errno));
        return -1;
    }

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(ipv4);

    if (connect(handle, (const struct sockaddr*) &addr, sizeof(addr)) == -1) {
        close(handle);
        log_error("Connecting to %X, %d failed: %s", ipv4, port, strerror(errno));
        return -1;
    }

    log_debug("Connected to %X on port %d", ipv4, port);

    return handle;
}

ssize_t util_sock_tcp_send_block(int handle, const void* buffer, size_t size)
{
    return send(handle, buffer, size, 0);
}

ssize_t util_sock_tcp_send(int handle, const void* buffer, size_t size, uint32_t timeout_ms)
{
    struct timeval tv;
    tv.tv_sec = (__time_t) (timeout_ms / 1000);
    tv.tv_usec = (__suseconds_t) ((timeout_ms % 1000) * 1000);

    setsockopt(handle, SOL_SOCKET, SO_SNDTIMEO, (const char*) &tv, sizeof(tv));

    ssize_t length = send(handle, buffer, size, 0);

    if (length < 0) {
        // same as EWOULDBLOCK
        if (errno == EAGAIN) {
            // no data for non blocking
            return 0;
        }

        return -1;
    }

    return length;
}

ssize_t util_sock_tcp_recv_block(int handle, void* buffer, size_t size)
{
    return recv(handle, buffer, size, 0);
}

ssize_t util_sock_tcp_recv(int handle, void* buffer, size_t size, uint32_t timeout_ms)
{
    struct timeval tv;
    tv.tv_sec = (__time_t) (timeout_ms / 1000);
    tv.tv_usec = (__suseconds_t) ((timeout_ms % 1000) * 1000);

    setsockopt(handle, SOL_SOCKET, SO_RCVTIMEO, (const char*) &tv, sizeof(tv));

    ssize_t length = recv(handle, buffer, size, 0);

    if (length < 0) {
        // same as EWOULDBLOCK
        if (errno == EAGAIN) {
            // no data for non blocking
            return 0;
        }

        return -1;
    } else if (length == 0) {
        // connection lost
        return -1;
    } else {
      return length;
    }
}

void util_sock_tcp_close(int handle)
{
    close(handle);
}