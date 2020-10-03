/**
 * Various functions related to library hooking
 */
#ifndef CAPNHOOK_LIB_H
#define CAPNHOOK_LIB_H

#include <stdlib.h>

/**
 * Provide a mock function key'd by it's name and pointing to your actual mock implementation.
 */
struct cnh_lib_unit_test_func_mocks {
    const char* name;
    void* func;
};

/**
 * Allocate memory for a list of mock functions to pass to cnh_lib_init_unit_test.
 *
 * @param count The size of the list.
 * @return Allocated memory with the specified number of entries.
 */
struct cnh_lib_unit_test_func_mocks* cnh_lib_allocate_func_mocks(size_t count);

/**
 * Initialize this module when using it in a unit-test. This allows you to add function mocks that are returned on the
 * various calls to get function pointers from real library functions. Use this to mock them to test various patching
 * modules.
 *
 * Use the cnh_lib_allocate_func_mocks function to allocate a list that you can fill in.
 *
 * @param func_mocks A list of functions mocks with function names and pointers to the test mocks you set up. Ensure
 *                   that the function signatures match, otherwise things might crash when getting called. The module
 *                   will cleanup the memory allocated of this for you.
 * @param func_mocks_cnt The length of the list of func_mocks.
 */
void cnh_lib_init_unit_test(struct cnh_lib_unit_test_func_mocks* func_mocks, size_t func_mocks_cnt);

/**
 * Optional, used for unit testing. Switch to the default backend that utilizes loading real libraries and getting real
 * function pointers from them. This is enabled by default.
 */
void cnh_lib_init();

/**
 * Cleanup any memory allocated/used on cnh_lib_init_unit_test.
 */
void cnh_lib_shutdown_unit_test();

/**
 * Load a dynamic library
 *
 * @param lib Path to library file to load
 * @return Pointer to a handle on success, NULL on failure
 */
void* cnh_lib_load(const char* lib);

/**
 * Get the address of a function from a library
 *
 * @param lib_handle Handle of an opened library
 * @param func_name Name of the function to get
 * @return Function ptr on success, false on error (function not found)
 */
void*cnh_lib_get_func_addr_handle(void* lib_handle, const char* func_name);

/**
 * Unload a loaded dynamic library
 *
 * @param handle Handle of the loaded lib to unload
 */
void cnh_lib_unload(void* handle);

/**
 * Get the adress of a function (from a dynamic library)
 *
 * Note: This will return the first function found matching the specified name
 *
 * @param func_name Name of the function
 * @return Valid ptr to function on success, false on error (not found)
 */
void* cnh_lib_get_func_addr(const char* func_name);

#endif