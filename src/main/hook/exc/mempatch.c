#include <stdlib.h>
#include <string.h>

#include "hook/exc/mempatch.h"

#include "util/defs.h"

static const struct exchook_mempatch_table _exchook_mempatch_table[] = {
    {
        .version = "20040325",
        .addr_eeprom_read = 0x080716E0,
        .addr_eeprom_write = 0x08071620,
        .addr_io_check = 0x0806DEB0,
        .addr_io_light_all = 0x0806E5A0,
        .addr_io_light_neon = 0x0806E600,
        .addr_io_light_halogan = 0x0806E630,
        .addr_io_light_p1_ccfl = 0x0806E660,
        .addr_io_light_p2_ccfl = 0x0806E690,
        .addr_io_input = 0x0806E6C0,
        .addr_io_coin_input = 0x0806EE20,
        .addr_io_coin_input_2 = 0x0806EEE0,
        .addr_io_coin_counter_2 = 0x0806EFA0,
        .addr_io_game_input_stat = 0x080B9410,
        .addr_io_game_input_down = 0x080B9408,
        .addr_io_game_input_up = 0x080B940C,
        .addr_lockchip_dec_chunk = 0x08071E80,
        .addr_stop_all_effects_return = 0x0806F6E0,
    },
    {
        .version = "20040408",
        .addr_eeprom_read = 0x08071CC0,
        .addr_eeprom_write = 0x08071C00,
        .addr_io_check = 0x0806E550,
        .addr_io_light_all = 0x0806EC40,
        .addr_io_light_neon = 0x0806ECA0,
        .addr_io_light_halogan = 0x0806ECD0,
        .addr_io_light_p1_ccfl = 0x0806ED00,
        .addr_io_light_p2_ccfl = 0x0806ED30,
        .addr_io_input = 0x0806ED60,
        .addr_io_coin_input = 0x0806F4C0,
        .addr_io_coin_input_2 = 0x0806F580,
        .addr_io_coin_counter_2 = 0x0806F640,
        .addr_io_game_input_stat = 0x080B5290,
        .addr_io_game_input_down = 0x080B5288,
        .addr_io_game_input_up = 0x080B528C,
        .addr_lockchip_dec_chunk = 0x08072460,
        .addr_stop_all_effects_return = 0x0806FD80,
    },
};

const struct exchook_mempatch_table* exchook_mempatch_get_table(const char* version)
{
    size_t entries = lengthof(_exchook_mempatch_table);

    for (uint32_t i = 0; i < entries; i++) {
        if (!strcmp(_exchook_mempatch_table[i].version, version)) {
            return &_exchook_mempatch_table[i];
        }
    }

    return NULL;
}