#define LOG_MODULE "util-patch"

#include <sys/mman.h>

#include "util/log.h"
#include "util/patch.h"

/* addr needs to be a multiple of page size */
#define UNPROTECT(addr, len)           \
  mprotect(                            \
      (void *) (addr - (addr % 4096)), \
      (addr % 4096) + len,             \
      PROT_READ | PROT_WRITE | PROT_EXEC)

void util_patch_write_memory(uintptr_t mem, const void *data, size_t len)
{
  if (UNPROTECT(mem, len) != 0) {
    log_die("Unprotecting memory %p, len %d failed", mem, len);
  }

  memcpy((void *) mem, data, len);
}

void util_patch_write_memory_byte(uintptr_t mem, uint8_t data)
{
  util_patch_write_memory(mem, (const void *) &data, sizeof(uint8_t));
}

void util_patch_function(uintptr_t func_addr, void *detour_func)
{
  /* uintptr_t is said to be portable (32/64-bit) */
  uint8_t offset = 5;

  /* 64-bit binary */
  if (sizeof(uintptr_t) == 8) {
    offset += 4;
  }

  uintptr_t call = (uintptr_t)(((uintptr_t) detour_func) - func_addr - offset);

  if (UNPROTECT(func_addr, 4096) != 0) {
    log_die("Unprotecting memory %p, len %d failed", func_addr);
  }

  *((uint8_t *) (func_addr)) = 0xE9;
  *((uintptr_t *) (func_addr + 1)) = call;
}

void *util_patch_find_signiture(
    const uint8_t *sig,
    uint32_t sig_len,
    int32_t sig_offset,
    void *start_addr,
    void *end_addr,
    int32_t alignment)
{
  size_t pos;

  for (pos = (size_t) start_addr; pos < (size_t) end_addr; pos += alignment) {
    if (!memcmp((void *) pos, sig, sig_len)) {
      return (void *) (pos + sig_offset);
    }
  }

  return NULL;
}