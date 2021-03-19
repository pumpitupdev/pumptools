#define LOG_MODULE "test-util-mem"

#include <cmocka/cmocka.h>

#include "test-util/mem.h"

#include "util/log.h"
#include "util/mem.h"

static void *_test_util_mem_xmalloc(size_t nbytes)
{
  void *ptr;

  ptr = test_malloc(nbytes);

  log_debug("malloc(%d): %p", nbytes, ptr);

  return ptr;
}

static void *_test_util_mem_xrealloc(void *mem, size_t nbytes)
{
  void *ptr;

  ptr = test_realloc(mem, nbytes);

  log_debug("realloc(%p %d): %p", mem, nbytes, ptr);

  return ptr;
}

static void _test_util_mem_xfree(void **mem)
{
  if (*mem != NULL) {
    log_debug("free(%p %p)", mem, *mem);
    test_free(*mem);
    *mem = NULL;
  } else {
    log_warn("free(%p NULL)", mem);
  }
}

static const struct util_mem_interface _test_util_mem_interface_test = {
    .xmalloc = _test_util_mem_xmalloc,
    .xrealloc = _test_util_mem_xrealloc,
    .xfree = _test_util_mem_xfree,
};

void test_util_mem_install_mem_interface()
{
  util_mem_init(&_test_util_mem_interface_test);
}