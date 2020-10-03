#ifndef UTIL_MEM_H
#define UTIL_MEM_H

#include <stddef.h>
#include <stdbool.h>

typedef void* (*util_xmalloc_t)(size_t nbytes);
typedef void* (*util_xrealloc_t)(void* mem, size_t nbytes);
typedef void (*util_free_t)(void** mem);

struct util_mem_interface {
    util_xmalloc_t xmalloc;
    util_xrealloc_t xrealloc;
    util_free_t xfree;
};

/**
 * Install a different memory interfacee, e.g. to use a different allocator or hooks for unit-testing.
 */
void util_mem_init(const struct util_mem_interface* interface);

/**
 * Optional, initialize the default memory interface. This needs to be called only if you want to switch back from
 * a non-default interface that got installed using util_mem_init.
 */
void util_mem_init_default();

/**
 * Enable debug output of all allocations and de-allocations that go through the default interface of this module.
 */
void util_mem_enable_debug_output();

/**
 * Allocate memory on the heap (malloc wrapper)
 *
 * @param nbytes Number of bytes to allocate
 * @return Pointer to allocated memory
 */
void* util_xmalloc(size_t nbytes);

/**
 * Realloc memory (realloc wrapper)
 *
 * @param mem Pointer to allocated memory to realloc/expand
 * @param nbytes New size of memory region
 * @return Pointer to reallocated memory region
 */
void* util_xrealloc(void* mem, size_t nbytes);

/**
 * Free allocated memory.
 *
 * @param mem Pointer to the pointer with allocated memory to free. The pointer to the memory will be set to NULL.
 */
void util_xfree(void** mem);

#endif
