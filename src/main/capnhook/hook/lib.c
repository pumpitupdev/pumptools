#define LOG_MODULE "hook-lib"

#include <dlfcn.h>

#include "capnhook/hook/lib.h"

#include "util/log.h"
#include "util/mem.h"
#include "util/rand.h"

typedef void *(*cnh_lib_load_t)(const char *lib);
typedef void *(*cnh_lib_get_func_addr_handle_t)(
    void *lib_handle, const char *func_name);
typedef void (*cnh_lib_unload_t)(void *handle);
typedef void *(*cnh_lib_get_func_addr_t)(const char *func_name);

static void *_cnh_lib_load(const char *lib);
static void *
_cnh_lib_get_func_addr_handle(void *lib_handle, const char *func_name);
static void _cnh_lib_unload(void *handle);
static void *_cnh_lib_get_func_addr(const char *func_name);

static void *_cnh_lib_load_test(const char *lib);
static void *
_cnh_lib_get_func_addr_handle_test(void *lib_handle, const char *func_name);
static void _cnh_lib_unload_test(void *handle);
static void *_cnh_lib_get_func_addr_test(const char *func_name);

struct cnh_lib_interface {
  cnh_lib_load_t load;
  cnh_lib_get_func_addr_handle_t get_func_addr_handle;
  cnh_lib_unload_t unload;
  cnh_lib_get_func_addr_t get_func_addr;
};

static struct cnh_lib_interface _cnh_lib_interface = {
    .load = _cnh_lib_load,
    .get_func_addr_handle = _cnh_lib_get_func_addr_handle,
    .unload = _cnh_lib_unload,
    .get_func_addr = _cnh_lib_get_func_addr,
};

static struct cnh_lib_unit_test_func_mocks *_cnh_lib_func_mocks;
size_t _cnh_lib_func_mocks_cnt;

static void *_cnh_lib_load(const char *lib)
{
  void *so = dlopen(lib, RTLD_NOW);

  if (so == NULL) {
    log_warn("Failed to open library %s: %s", lib, dlerror());
  } else {
    log_debug("Opened lib %s, handle %p", lib, so);
  }

  return so;
}

static void *
_cnh_lib_get_func_addr_handle(void *lib_handle, const char *func_name)
{
  void *ret = dlsym(lib_handle, func_name);

  if (ret == NULL) {
    log_warn("Could not find func %s in library %p", func_name, lib_handle);
  }

  return ret;
}

static void _cnh_lib_unload(void *handle)
{
  dlclose(handle);
  log_debug("Closed lib handle %p", handle);
}

static void *_cnh_lib_get_func_addr(const char *func_name)
{
  void *ret = dlsym(RTLD_NEXT, func_name);

  if (ret == NULL) {
    log_warn("Could not find func %s", func_name);
  }

  return ret;
}

static void *_cnh_lib_load_test(const char *lib)
{
  log_warn("Loading lib %s not supported in test", lib);

  if (sizeof(void *) == 4) {
    return (void *) util_rand_gen_32();
  } else {
    log_die_illegal_state();
  }
}

static void *
_cnh_lib_get_func_addr_handle_test(void *lib_handle, const char *func_name)
{
  return _cnh_lib_get_func_addr_test(func_name);
}

static void _cnh_lib_unload_test(void *handle)
{
  log_warn("Unloading lib %p not supported in test", handle);
}

static void *_cnh_lib_get_func_addr_test(const char *func_name)
{
  for (size_t i = 0; i < _cnh_lib_func_mocks_cnt; i++) {
    if (!strcmp(_cnh_lib_func_mocks[i].name, func_name)) {
      return _cnh_lib_func_mocks->func;
    }
  }

  log_warn("Could not find func %s", func_name);

  return NULL;
}

struct cnh_lib_unit_test_func_mocks *cnh_lib_allocate_func_mocks(size_t count)
{
  log_assert(count >= 0);

  struct cnh_lib_unit_test_func_mocks *func_mocks;

  func_mocks =
      util_xmalloc(sizeof(struct cnh_lib_unit_test_func_mocks) * count);

  memset(func_mocks, 0, sizeof(struct cnh_lib_unit_test_func_mocks) * count);

  return func_mocks;
}

void cnh_lib_init_unit_test(
    struct cnh_lib_unit_test_func_mocks *func_mocks, size_t func_mocks_cnt)
{
  log_warn("Setting up %d unit test func mocks", func_mocks_cnt);

  _cnh_lib_func_mocks = func_mocks;
  _cnh_lib_func_mocks_cnt = func_mocks_cnt;

  _cnh_lib_interface.load = _cnh_lib_load_test;
  _cnh_lib_interface.get_func_addr_handle = _cnh_lib_get_func_addr_handle_test;
  _cnh_lib_interface.unload = _cnh_lib_unload_test;
  _cnh_lib_interface.get_func_addr = _cnh_lib_get_func_addr_test;
}

void cnh_lib_init()
{
  log_warn("Setting up real interface");

  _cnh_lib_interface.load = _cnh_lib_load;
  _cnh_lib_interface.get_func_addr_handle = _cnh_lib_get_func_addr_handle;
  _cnh_lib_interface.unload = _cnh_lib_unload;
  _cnh_lib_interface.get_func_addr = _cnh_lib_get_func_addr;
}

void cnh_lib_shutdown_unit_test()
{
  log_assert(_cnh_lib_func_mocks);

  util_xfree((void **) &_cnh_lib_func_mocks);
}

void *cnh_lib_load(const char *lib)
{
  return _cnh_lib_interface.load(lib);
}

void *cnh_lib_get_func_addr_handle(void *lib_handle, const char *func_name)
{
  return _cnh_lib_interface.get_func_addr_handle(lib_handle, func_name);
}

void cnh_lib_unload(void *handle)
{
  _cnh_lib_interface.unload(handle);
}

void *cnh_lib_get_func_addr(const char *func_name)
{
  return _cnh_lib_interface.get_func_addr(func_name);
}