#ifndef UTIL_PATCH_H
#define UTIL_PATCH_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * Write data to an arbitrary memory location
 *
 * @param mem Memory location to write to
 * @param data Pointer to buffer with data to write
 * @param len Length of data to write
 */
void util_patch_write_memory(uintptr_t mem, const void *data, size_t len);

/**
 * Write a single byte to an arbitrary memory location
 *
 * @param mem Memory location to write to
 * @param data Byte to write
 */
void util_patch_write_memory_byte(uintptr_t mem, uint8_t data);

/**
 * Write a function ptr/function call to a memory location
 *
 * Use this to patch existing call instructions and detour to your own code
 *
 * @param func_addr Address of the function/call instruction to replace
 * @param detour_func Address of your function to detour to
 */
void util_patch_function(uintptr_t func_addr, void *detour_func);

/**
 * Scan a memory area limited by a start and end address for a signiture.
 *
 * @param sig Buffer with signiture to search for
 * @param sig_len Length of signiture
 * @param sig_offset Offset that gets added to the address where the signiture
 *                   starts (if found)
 * @param start_addr Pointer to the address to start searching at
 * @param end_addr Pointer to the address to stop searching at
 * @param alignment Aligns the search signiture, i.e. search in steps of
 *                  X bytes
 * @return If found, pointer to memory where the signiture was found (first
 *         occurance) with the signiture offset added, NULL otherwise
 */
void *util_patch_find_signiture(
    const uint8_t *sig,
    uint32_t sig_len,
    int32_t sig_offset,
    void *start_addr,
    void *end_addr,
    int32_t alignment);

#endif