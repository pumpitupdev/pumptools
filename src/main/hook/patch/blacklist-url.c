#define LOG_MODULE "patch-blacklist-url"

#include <string.h>

#include "capnhook/hook/lib.h"

#include "util/list.h"
#include "util/log.h"
#include "util/mem.h"

struct patch_blacklist_url_list_elem {
  struct util_list_node head;
  const char *url;
};

typedef int (*curl_easy_setopt_t)(int *handle, int option, void *param);

static bool patch_blacklist_url_initialized;
static curl_easy_setopt_t patch_blacklist_url_real_curl_easy_setopt;
static struct util_list patch_blacklist_url_list;

int curl_easy_setopt(int *handle, int option, void *param)
{
  if (!patch_blacklist_url_real_curl_easy_setopt) {
    patch_blacklist_url_real_curl_easy_setopt =
        (curl_easy_setopt_t) cnh_lib_get_func_addr("curl_easy_setopt");
  }

  if (patch_blacklist_url_initialized) {

    /* option == 10002 to identify that param has a string */
    if (option == 10002) {

      struct util_list_node *pos;
      struct patch_blacklist_url_list_elem *elem;

      for (pos = patch_blacklist_url_list.head; pos != NULL; pos = pos->next) {
        elem = containerof(pos, struct patch_blacklist_url_list_elem, head);

        if (!memcmp(param, elem->url, strlen(elem->url))) {
          log_debug("Blocking url %s", elem->url);
          return patch_blacklist_url_real_curl_easy_setopt(
              handle, option, (void *) "does-not-exist");
        }
      }
    }
  }

  return patch_blacklist_url_real_curl_easy_setopt(handle, option, param);
}

void patch_blacklist_url_init(void)
{
  util_list_init(&patch_blacklist_url_list);
  patch_blacklist_url_initialized = true;
  log_info("Initialized");
}

void patch_blacklist_url_add(const char *url)
{
  struct patch_blacklist_url_list_elem *elem;

  elem = util_xmalloc(sizeof(struct patch_blacklist_url_list_elem));
  elem->url = strdup(url);

  util_list_append(&patch_blacklist_url_list, &elem->head);
  log_info("Add url %s to blacklist", url);
}