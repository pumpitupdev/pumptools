#define LOG_MODULE "exchook-lockchip"

#include <stdlib.h>

#include "sec/lockchip/lockchip.h"
#include "sec/lockchip/lockchip-defs.h"

#include "util/log.h"
#include "util/patch.h"

#include "lockchip.h"

static void exchook_lockchip_dec_chunk(uint8_t len, const uint8_t* src,
    uint8_t* dest);

void exchook_lockchip_init(const struct exchook_mempatch_table* patch_table)
{
    sec_lockchip_init(
        sec_lockchip_defs_transforms[SEC_LOCK_CHIP_DEFS_GAME_EXCEED]);

    /* Hook lockchip */
    util_patch_function(patch_table->addr_lockchip_dec_chunk, exchook_lockchip_dec_chunk);

    log_info("Initialized");
}

static void exchook_lockchip_dec_chunk(uint8_t len, const uint8_t* src,
        uint8_t* dest)
{
 	sec_lockchip_start();

	uint8_t last_res = sec_lockchip_step(src[0] ^ 0xFF);

    for (size_t i = 1; i < len; i++)
    {
    	uint8_t result = sec_lockchip_step(src[i] ^ 0xFF);
    	dest[i - 1] = (uint8_t)((last_res >> 3) ^ (result << 5));
    	last_res = result;
    }

    uint8_t last_byte = sec_lockchip_step(0);
    dest[len - 1] = (uint8_t)((last_res >> 3) ^ (last_byte << 5));
}