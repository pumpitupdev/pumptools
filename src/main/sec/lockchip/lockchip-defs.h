#ifndef SEC_LOCKCHIP_DEFS_H
#define SEC_LOCKCHIP_DEFS_H

#include <stdint.h>

/* List of games using the old type mk3 lockchip board */
enum sec_lockchip_defs_games {
    SEC_LOCK_CHIP_DEFS_GAME_3RD_SE = 0,
    SEC_LOCK_CHIP_DEFS_GAME_COLLECTION = 1,
    SEC_LOCK_CHIP_DEFS_GAME_PERFECT_COLLECTION = 2,
    SEC_LOCK_CHIP_DEFS_GAME_EXTRA_PROTO = 3,
    SEC_LOCK_CHIP_DEFS_GAME_PREMIERE = 4,
    SEC_LOCK_CHIP_DEFS_GAME_PREX = 5,
    SEC_LOCK_CHIP_DEFS_GAME_REBIRTH = 6,
    SEC_LOCK_CHIP_DEFS_GAME_PREMIERE_2 = 7,
    /* Extra Plus */
    SEC_LOCK_CHIP_DEFS_GAME_PREX_2_BETA = 8,
    SEC_LOCK_CHIP_DEFS_GAME_PREX_2 = 9,
    SEC_LOCK_CHIP_DEFS_GAME_PREMIERE_3 = 10,
    SEC_LOCK_CHIP_DEFS_GAME_PREX_3 = 11,
    SEC_LOCK_CHIP_DEFS_GAME_EXTRA = 12,
    /* NXA for mk3, funbox... */
    SEC_LOCK_CHIP_DEFS_GAME_BOOTLEGS = 13, 
    SEC_LOCK_CHIP_DEFS_GAME_EXCEED = 14,
    SEC_LOCK_CHIP_DEFS_GAME_COUNT = 15,
};

/** List of lockchip transforms of all pump games that used it.
 */
static const uint8_t sec_lockchip_defs_transforms
        [SEC_LOCK_CHIP_DEFS_GAME_COUNT][8] = {
    /* 3rd SE */
	{0xF0, 0x1C, 0xFE, 0x03, 0x81, 0x40, 0x38, 0xF8},
	/* Collection */
    {0xF0, 0x1C, 0xFE, 0x03, 0x81, 0x40, 0x38, 0xF8},
	/* Perfect Collection */
    {0xF0, 0x1C, 0xFE, 0x03, 0x81, 0x40, 0x38, 0xF8},
	/* Extra Proto */
    {0xF8, 0x82, 0x70, 0x0C, 0xFD, 0xC1, 0x20, 0xE0}, 
	/* Premiere */
    {0xE0, 0x78, 0x7C, 0xFD, 0xF1, 0xF2, 0xC2, 0xC0},
	/* Prex */
    {0xE0, 0x78, 0x7C, 0xFD, 0xF1, 0xF2, 0xC2, 0xC0},
	/* Rebirth */
    {0xFC, 0xF9, 0x08, 0x10, 0xE2, 0xC2, 0x40, 0x80},
	/* Premiere 2 */
    {0xFC, 0x03, 0xE1, 0x20, 0x38, 0x78, 0x70, 0xF0},
	/* Prex 2 Beta */
    {0xE0, 0x38, 0x78, 0x81, 0x03, 0xFE, 0x0C, 0xF0},
	/* Prex 2 */
    {0xE0, 0x38, 0x78, 0x81, 0x03, 0xFE, 0x0C, 0xF0},
	/* Premiere 3 */
    {0x02, 0xC1, 0xF9, 0x78, 0x7C, 0x1C, 0x10, 0xF0},
	/* Prex 3 */
    {0xF0, 0xE1, 0xF9, 0x78, 0x7C, 0xFE, 0xC2, 0xC0},
	/* Extra */
    {0xF8, 0x82, 0x70, 0x0C, 0xFD, 0xC1, 0x20, 0xE0},
	/* Bootlegs */
    {0x02, 0xC1, 0xF9, 0x78, 0x7C, 0x1C, 0x10, 0xF0},
	/* Exceed */
    {0xF0, 0x78, 0xF9, 0xFD, 0x1C, 0x20, 0xC2, 0x02},
};

#endif