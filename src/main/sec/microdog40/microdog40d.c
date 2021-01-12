#define LOG_MODULE "sec-microdog40d"

#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>

#include "sec/microdog40/microdog40.h"

#include "util/log.h"

static const char* sec_microdog40d_sock_path = "/var/run/microdog/u.daemon";

static bool sec_microdog40d_run_d;
static bool sec_microdog40d_is_running_d;
struct timeval tv;

void sec_microdog40d_init(const uint8_t* key_data, size_t len)
{
    sec_microdog40_init(key_data, len);
    sec_microdog40d_run_d = true;
    sec_microdog40d_is_running_d = false;
}

void sec_microdog40d_run(void)
{
    int fd;
    struct sec_microdog40_transaction transaction;
    struct sockaddr_un peer_addr;
    socklen_t peer_addr_len;
    int retval;

    sec_microdog40d_is_running_d = true;

    fd = -1;
    peer_addr_len = sizeof(peer_addr);
    retval = 0;

    unlink(sec_microdog40d_sock_path);

    /* make sure /var/run/microdog folder exists */
    mkdir("/var/run/microdog", 0700);

    fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (fd == -1) {
        log_error("Opening socket failed: %s", strerror(errno));
        sec_microdog40d_is_running_d = false;
        return;
    }

    tv.tv_sec=5;
    tv.tv_usec = 0;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
        sec_microdog40d_is_running_d = false;
        return;
    }

    memset(&peer_addr, 0, sizeof(struct sockaddr_un));
    peer_addr.sun_family = AF_UNIX;
    strcpy(peer_addr.sun_path, sec_microdog40d_sock_path);

    retval = bind(fd, (struct sockaddr *) &peer_addr,
        sizeof(struct sockaddr_un));

    if (retval == -1) {
        log_error("Binding socket failed: %s", strerror(errno));
        sec_microdog40d_is_running_d = false;
        return;
    }

    while (sec_microdog40d_run_d) {
        peer_addr_len = sizeof(peer_addr);
        retval = recvfrom(fd, &transaction,
            sizeof(struct sec_microdog40_transaction), 0,
            (struct sockaddr *) &peer_addr, &peer_addr_len);

        if (retval == -1) {

            if (errno == EBADF) {
                break;
            } else if (errno == EAGAIN) {
                continue;
            } else {
                log_error("Receiving data failed: %s", strerror(errno));
                sec_microdog40d_is_running_d = false;
                return;
            }
        } else if (retval != sizeof(struct sec_microdog40_transaction)) {
            log_warn("Package too short, expected %d bytes, got %d bytes",
                sizeof(struct sec_microdog40_transaction), retval);
        } else {
            if (sec_microdog40_do_transaction(&transaction)) {
                sendto(fd, &transaction,
                    sizeof(struct sec_microdog40_transaction), 0,
                    (struct sockaddr *) &peer_addr, peer_addr_len);
            } else {
                log_error("Transaction failed.");
            }
        }
    }

    // cleanup
    close(fd);
    unlink(sec_microdog40d_sock_path);

    sec_microdog40d_is_running_d = false;
}

bool sec_microdog40d_is_running(void)
{
    return sec_microdog40d_is_running_d;
}

void sec_microdog40d_shutdown(void)
{
    sec_microdog40d_run_d = false;
}
