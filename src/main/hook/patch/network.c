#define LOG_MODULE "patch-network"

#include <string.h>
#include <arpa/inet.h>

#include "capnhook/hook/lib.h"

#include "util/list.h"
#include "util/log.h"
#include "util/mem.h"
#include "util/str.h"

struct patch_network_redirect_elem {
    struct util_list_node head;
    char* orig_ipv4;
    char* new_ipv4;
    uint16_t new_port;
};

typedef int (*connect_t) (int __fd, __CONST_SOCKADDR_ARG __addr, socklen_t __len);

static connect_t patch_network_real_connect;

static bool patch_network_initialized;
static struct util_list patch_network_redirect_list;

int connect(int fd, __CONST_SOCKADDR_ARG addr, socklen_t len)
{
    if (!patch_network_real_connect) {
        patch_network_real_connect = (connect_t) cnh_lib_get_func_addr("connect");
    }

    if (patch_network_initialized) {
        if (addr.__sockaddr__->sa_family == AF_INET) {
            log_debug("connect ipv4 %d.%d.%d.%d:%d",
                addr.__sockaddr_in__->sin_addr.s_addr & 0xFF,
                addr.__sockaddr_in__->sin_addr.s_addr >> 8 & 0xFF,
                addr.__sockaddr_in__->sin_addr.s_addr >> 16 & 0xFF,
                addr.__sockaddr_in__->sin_addr.s_addr >> 24 & 0xFF,
            addr.__sockaddr_in__->sin_port);

            struct util_list_node* pos;
            struct patch_network_redirect_elem* elem;
            struct sockaddr_in tmp;

            for (pos = patch_network_redirect_list.head; pos != NULL; pos = pos->next) {
                elem = containerof(pos, struct patch_network_redirect_elem, head);

                inet_aton(elem->orig_ipv4, &tmp.sin_addr);

                if (addr.__sockaddr_in__->sin_addr.s_addr == tmp.sin_addr.s_addr) {
                    char* tmp_old = inet_ntoa(addr.__sockaddr_in__->sin_addr);
                    log_info("Redirect %s:%d -> %s:%d", tmp_old,
                        ((struct sockaddr_in*) addr.__sockaddr_in__)->sin_port, elem->new_ipv4,
                        elem->new_port != 0xFFFF ? elem->new_port :
                            ((struct sockaddr_in*) addr.__sockaddr_in__)->sin_port);

                    inet_aton(elem->new_ipv4, &((struct sockaddr_in*) addr.__sockaddr_in__)->sin_addr);

                    if (elem->new_port != 0xFFFF) {
                        ((struct sockaddr_in*) addr.__sockaddr_in__)->sin_port = htons(elem->new_port);
                    }
                }
            }
        } else {
            log_debug("connect, addr family: %d", addr.__sockaddr__->sa_family);
        }
    }

    return patch_network_real_connect(fd, addr, len);
}

void patch_network_init()
{
    util_list_init(&patch_network_redirect_list);
    patch_network_initialized = true;
    log_info("Initialized");
}

void patch_network_redirect_server_address(const char* orig_ipv4, const char* new_ipv4, uint16_t new_port)
{
    struct patch_network_redirect_elem* elem;

    elem = util_xmalloc(sizeof(struct patch_network_redirect_elem));
    elem->orig_ipv4 = util_str_dup(orig_ipv4);
    elem->new_ipv4 = util_str_dup(new_ipv4);
    elem->new_port = new_port;

    util_list_append(&patch_network_redirect_list, &elem->head);

    if (new_port != 0xFFFF) {
        log_info("Add network redirect: %s -> %s:%d", orig_ipv4, new_ipv4, new_port);
    } else {
        log_info("Add network redirect: %s -> %s", orig_ipv4, new_ipv4);
    }
}
