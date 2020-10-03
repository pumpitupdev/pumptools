#define LOG_MODULE "sec-hasp-server"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <unistd.h>

#include "sec/hasp/lib/handler.h"

#include "util/log.h"

#include "server.h"

#define HASP_PORT 1947
#define HASP_MAX_BUFFER 16384

static bool sec_hasp_server_run_s;
static bool sec_hasp_server_is_running_s;

void sec_hasp_server_init(const uint8_t* key_data, size_t len)
{
    sec_hasp_handler_init(key_data, len);
    sec_hasp_server_run_s = true;
    sec_hasp_server_is_running_s = false;
}

void sec_hasp_server_run()
{
    int fd = -1;
    struct sockaddr_in addr;
    struct sockaddr_in addr_remote;
    socklen_t addr_len_remote;
    int fd_remote = -1;
    int retval;
    struct sec_hasp_handler_transaction* transaction;
    ssize_t recv_len;

    log_info("Setting up socket for server");

    sec_hasp_server_is_running_s = true;

    transaction = sec_hasp_handler_alloc_transaction(HASP_MAX_BUFFER);

    fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd == -1) {
        log_error("Opening socket failed: %s", strerror(errno));
        goto return_cleanup;
    }

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    /* bind to localhost which limits accepting connections to localhost, only */
    addr.sin_addr.s_addr = htonl(0x7F000001);
    addr.sin_port = htons(HASP_PORT);

    retval = bind(fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));

    if (retval == -1) {
        log_error("Binding socket failed: %s", strerror(errno));
        goto return_cleanup;
    }

    log_info("Running server...");

    while (sec_hasp_server_run_s) {
        /* a single connection is fine here */
        retval = listen(fd, 1);

        if (retval == -1) {
            log_error("Listening for incoming connections failed: %s", strerror(errno));
            goto return_cleanup;
        }

        /* got a connection */
        addr_len_remote = sizeof(addr_remote);
        fd_remote = accept(fd, &addr_remote, &addr_len_remote);

        if (fd_remote == -1) {
            log_error("Accepting incoming connection failed: %s", strerror(errno));
            goto return_cleanup;
        }

        log_debug("Remote %X connected", addr_remote.sin_addr.s_addr);

        /* put the socket to the remote into non blocking mode */
        if (fcntl(fd_remote, F_SETFL, fcntl(fd_remote, F_GETFL) | O_NONBLOCK) < 0) {
            log_error("Setting socket to non blocking failed: %s", strerror(errno));
            goto return_cleanup;
        }

        while (sec_hasp_server_run_s) {
            recv_len = recv(fd_remote, transaction->req, transaction->req_resp_max_size, 0);

            if (recv_len == -1) {
                /* no data available, sleep to reduce cpu load and retry */
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    /* 10 ms */
                    usleep(1000 * 10);
                    continue;
                } else if (errno == ECONNRESET) {
                    log_debug("Remote %X disconnected", addr_remote.sin_addr.s_addr);
                    close(fd_remote);
                    break;
                } else {
                    log_error("Reading from connection failed: %s", strerror(errno));
                    goto return_cleanup;
                }
            }

            if (recv_len == 0) {
                /* remote disconnected */
                log_debug("Remote %X disconnected", addr_remote.sin_addr.s_addr);
                close(fd_remote);
                break;
            }

            log_debug("Received data: %d bytes", recv_len);

            sec_hasp_handler_do_transaction(transaction);

            /* send reply */
            recv_len = send(fd_remote, transaction->resp, transaction->resp->header.packet_size, 0);

            if (recv_len == -1) {
                if (errno == ECONNRESET) {
                    log_debug("Remote %X disconnected", addr_remote.sin_addr.s_addr);
                    close(fd_remote);
                    break;
                } else {
                    log_error("Writing to connection failed: %s", strerror(errno));
                    goto return_cleanup;
                }
            }
        }
    }

return_cleanup:
    if (fd_remote != -1) {
        close(fd);
    }

    if (fd != -1) {
        close(fd);
    }

    sec_hasp_handler_free_transaction(transaction);

    sec_hasp_server_is_running_s = false;
}

bool sec_hasp_server_is_running(void)
{
    return sec_hasp_server_is_running_s;
}

void sec_hasp_server_shutdown(void)
{
    sec_hasp_server_run_s = false;
}