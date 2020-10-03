#ifndef UTIL_ARRAY_H
#define UTIL_ARRAY_H

#include <stddef.h>

/**
 * Array with a fixed number of items
 */
struct util_array {
    void* items;
    size_t nitems;
    size_t nalloced;
};

#define util_array_item(type, array, i) \
        (&(((type*) (array)->items)[i]))

#define util_array_reserve(type, array, nitems) \
        ((type*) util_array_reserve_(sizeof(type), array, nitems))

#define util_array_remove(type, array, i) \
        util_array_remove_(sizeof(type), array, i)

#define util_array_append(type, array) \
        util_array_reserve(type, array, 1)

/**
 * Initialize the array after allocation
 *
 * @param array Pointer to allocated array to initialize
 */
void util_array_init(struct util_array* array);

/**
 * Reserve/allocate space for the specified number of items in the array
 *
 * @param itemsz Size of a single item
 * @param array Array to use
 * @param Number of items with itemsz each to reserve
 * @return Pointer to first index of reserved slots in array
 */
void* util_array_reserve_(size_t itemsz, struct util_array* array, size_t nitems);

/**
 * Remove an item from the array. The array will be shrunken to current_size - 1
 *
 * @param itemsz Size of a single item
 * @param array Array to remove the item from
 * @param i Index of the item to remove
 */
void util_array_remove_(size_t itemsz, struct util_array* array, size_t i);

/**
 * Free/Deallocate an allocated array
 *
 * @param array Allocated array to free
 */
void util_array_fini(struct util_array* array);

#endif
