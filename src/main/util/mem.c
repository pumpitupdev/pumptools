#define LOG_MODULE "util-mem"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "util/log.h"
#include "util/mem.h"

static bool _util_mem_debug_output;

static void* _util_xmalloc(size_t nbytes);
static void* _util_xrealloc(void* mem, size_t nbytes);
static void _util_xfree(void** mem);

static const struct util_mem_interface _util_mem_interface_default = {
    .xmalloc = _util_xmalloc,
    .xrealloc = _util_xrealloc,
    .xfree = _util_xfree,
};

static const struct util_mem_interface* _util_mem_interface = &_util_mem_interface_default;

static void* _util_xmalloc(size_t nbytes)
{
    void* mem;

    mem = malloc(nbytes);

    if (_util_mem_debug_output) {
        log_debug("xmalloc(%d): %p", nbytes, mem);
    }

    if (mem == NULL) {
        log_die("xmalloc(%u) failed", (uint32_t) nbytes);
        return NULL;
    }

    return mem;
}

static void* _util_xrealloc(void* mem, size_t nbytes)
{
    void* newmem;

    newmem = realloc(mem, nbytes);

    if (_util_mem_debug_output) {
        log_debug("xrealloc(%p %d): %p", mem, nbytes, newmem);
    }

    if (newmem == NULL) {
        log_die("xrealloc(%p, %u) failed", mem, (uint32_t) nbytes);
        return NULL;
    }

    return newmem;
}

static void _util_xfree(void** mem)
{
    log_assert(mem);

    if (*mem != NULL) {
        if (_util_mem_debug_output) {
            log_debug("xfree(%p %p)", mem, *mem);
        }

        free(*mem);
        *mem = NULL;
    } else {
        if (_util_mem_debug_output) {
            log_warn("xfree(%p NULL)", mem);
        }
    }
}

void util_mem_init(const struct util_mem_interface* interface)
{
    log_assert(interface);

    _util_mem_interface = interface;
}

void util_mem_init_default()
{
    _util_mem_interface = &_util_mem_interface_default;
}

void util_mem_enable_debug_output()
{
    _util_mem_debug_output = true;
}

void* util_xmalloc(size_t nbytes)
{
    return _util_mem_interface->xmalloc(nbytes);
}

void* util_xrealloc(void* mem, size_t nbytes)
{
    return _util_mem_interface->xrealloc(mem, nbytes);
}

void util_xfree(void** mem)
{
    _util_mem_interface->xfree(mem);
}