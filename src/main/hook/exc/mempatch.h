#ifndef EXCHOOK_MEMPATCH_H
#define EXCHOOK_MEMPATCH_H

#include <stdint.h>

struct exchook_mempatch_table {
    const char* version;
    uintptr_t addr_eeprom_read;
    uintptr_t addr_eeprom_write;
    uintptr_t addr_io_check;
    uintptr_t addr_io_light_all;
    uintptr_t addr_io_light_neon;
    uintptr_t addr_io_light_halogan;
    uintptr_t addr_io_light_p1_ccfl;
    uintptr_t addr_io_light_p2_ccfl;
    uintptr_t addr_io_input;
    uintptr_t addr_io_coin_input;
    uintptr_t addr_io_coin_input_2;
    uintptr_t addr_io_coin_counter_2;
    uintptr_t addr_io_game_input_stat;
    uintptr_t addr_io_game_input_down;
    uintptr_t addr_io_game_input_up;
    uintptr_t addr_lockchip_dec_chunk;
    uintptr_t addr_stop_all_effects_return;
};

const struct exchook_mempatch_table* exchook_mempatch_get_table(const char* version);

#endif