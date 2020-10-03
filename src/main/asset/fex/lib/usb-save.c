#include <stdio.h>
#include <string.h>

#include "util/adler32.h"
#include "util/mem.h"
#include "util/str.h"

#include "usb-save.h"

static char* fex_profile_save_header_to_string(
        const struct asset_fex_usb_save_header* header)
{
    char* buffer;

    buffer = (char*) util_xmalloc(256);
    memset(buffer, 0, 256);

    util_str_format(buffer, 256,
        "-------------- header --------------\n"
        "adler32: %X\n"
        "usb_serial: %s\n"
        "dongle_serial: %X\n",
        header->adler32,
        header->usb_serial,
        header->dongle_serial);

    return buffer;
}

static char* fex_profile_save_player_to_string(
        const struct asset_fex_usb_save_player* player)
{
    char* buffer;

    buffer = (char*) util_xmalloc(1024);
    memset(buffer, 0, 1024);

    util_str_format(buffer, 1024,
        "-------------- player --------------\n"
        "unkn: %X\n"
        "unkn2: %X\n"
        "avatar_id: %d\n"
        "player_lvl: %d\n"
        "unkn3: %X\n"
        "player_id: %s\n"
        "unkn4: %X\n"
        "unkn5: %X\n"
        "unkn6: %X\n"
        "unkn7: %X\n"
        "unkn8: %X\n"
        "unkn9: %X\n"
        "unkn10: %X\n"
        "unkn11: %X\n"
        "unkn12: %X\n"
        "unkn13: %X\n"
        "unkn14: %X\n"
        "unkn15: %X\n"
        "total_step: %d\n"
        "play_count: %d\n"
        "exp: %d\n"
        "num_quest_world_passed: %d\n"
        "unkn16: %X\n"
        "arcade_score: %d\n"
        "num_battle_wins: %d\n"
        "num_battle_loss: %d\n"
        "num_battle_draw: %d\n",
        player->unkn,
        player->unkn2,
        player->avatar_id,
        player->player_lvl,
        player->unkn3,
        player->player_id,
        player->unkn4,
        player->unkn5,
        player->unkn6,
        player->unkn7,
        player->unkn8,
        player->unkn9,
        player->unkn10,
        player->unkn11,
        player->unkn12,
        player->unkn13,
        player->unkn14,
        player->unkn15,
        player->total_step,
        player->play_count,
        player->exp,
        player->num_quest_world_passed,
        player->unkn16,
        player->arcade_score,
        player->num_battle_wins,
        player->num_battle_loss,
        player->num_battle_draw);

    return buffer;
}

static char* fex_profile_save_mods_to_string(
        const struct asset_fex_usb_save_player_mods* mods)
{
    char* buffer;

    buffer = (char*) util_xmalloc(128);
    memset(buffer, 0, 128);

    util_str_format(buffer, 128,
        "-------------- modifiers --------------\n"
        "modifiers: %X\n"
        "half_speed_mod: %X\n"
        "noteskin: %X\n",
        mods->modifiers,
        mods->half_speed_mod,
        mods->noteskin);

    return buffer;
}

static char* fex_profile_save_mod_unlocks_to_string(
        const struct asset_fex_usb_save_mod_unlocks* unlocks)
{
    char* merged;
    char* buffer;
    char* tmp;
    const size_t buf_size = 512;

    merged = (char*) util_xmalloc(buf_size);
    memset(merged, 0, buf_size);

    buffer = (char*) util_xmalloc(buf_size);
    memset(buffer, 0, buf_size);

    util_str_format(buffer, buf_size,
        "-------------- mod unlocks --------------\n" );

    tmp = util_str_merge(merged, buffer);
    free(merged);
    merged = tmp;

    for (uint32_t i = 0; i < 0x12; i++) {
        util_str_format(buffer, buf_size,
            "unkn0[%d]: %d\n",
            i,
            unlocks->unkn0[i]);

        tmp = util_str_merge(merged, buffer);

        free(merged);
        merged = tmp;
    }

    util_str_format(buffer, buf_size,
        "display_va: %d\n",
        unlocks->display_va);

    tmp = util_str_merge(merged, buffer);
    free(merged);
    merged = tmp;

    for (uint32_t i = 0; i < 0x0D; i++) {
        util_str_format(buffer, buf_size,
            "unkn1[%d]: %d\n",
            i,
            unlocks->unkn1[i]);

        tmp = util_str_merge(merged, buffer);

        free(merged);
        merged = tmp;
    }

    for (uint32_t i = 0; i < 0x10; i++) {
        util_str_format(buffer, buf_size,
            "noteskins[%d]: %d\n",
            i,
            unlocks->noteskins[i]);

        tmp = util_str_merge(merged, buffer);

        free(merged);
        merged = tmp;
    }

    for (uint32_t i = 0; i < 0x04; i++) {
        util_str_format(buffer, buf_size,
            "unkn2[%d]: %d\n",
            i,
            unlocks->unkn2[i]);

        tmp = util_str_merge(merged, buffer);

        free(merged);
        merged = tmp;
    }

    for (uint32_t i = 0; i < 0x03; i++) {
        util_str_format(buffer, buf_size,
            "path[%d]: %d\n",
            i,
            unlocks->path[i]);

        tmp = util_str_merge(merged, buffer);

        free(merged);
        merged = tmp;
    }

    for (uint32_t i = 0; i < 0x06; i++) {
        util_str_format(buffer, buf_size,
            "unkn3[%d]: %d\n",
            i,
            unlocks->unkn3[i]);

        tmp = util_str_merge(merged, buffer);

        free(merged);
        merged = tmp;
    }

    util_str_format(buffer, buf_size,
        "xj: %d\n",
        unlocks->xj);

    tmp = util_str_merge(merged, buffer);
    free(merged);
    merged = tmp;

    for (uint32_t i = 0; i < 0x02; i++) {
        util_str_format(buffer, buf_size,
            "system[%d]: %d\n",
            i,
            unlocks->system[i]);

        tmp = util_str_merge(merged, buffer);

        free(merged);
        merged = tmp;
    }

    for (uint32_t i = 0; i < 0x08; i++) {
        util_str_format(buffer, buf_size,
            "rush[%d]: %d\n",
            i,
            unlocks->rush[i]);

        tmp = util_str_merge(merged, buffer);

        free(merged);
        merged = tmp;
    }

    for (uint32_t i = 0; i < 0x1A; i++) {
        util_str_format(buffer, buf_size,
            "unkn4[%d]: %d\n",
            i,
            unlocks->unkn4[i]);

        tmp = util_str_merge(merged, buffer);

        free(merged);
        merged = tmp;
    }

    return merged;
}

static char* fex_profile_save_mission_state_to_string(
        const struct asset_fex_usb_save_mission_state* state)
{
    char* buffer;

    buffer = (char*) util_xmalloc(128);
    memset(buffer, 0, 128);

    util_str_format(buffer, 128,
        "-------------- mission state --------------\n"
        "quest_world_location: %X\n"
        "skill_up_zone_location: %X\n",
        state->quest_world_location,
        state->skill_up_zone_location);

    return buffer;
}

struct asset_fex_usb_save* asset_fex_usb_save_new(void)
{
    return malloc(sizeof(struct asset_fex_usb_save));
}

void asset_fex_usb_save_finalize(struct asset_fex_usb_save* save)
{

}

char* asset_fex_usb_save_to_string(const struct asset_fex_usb_save* save)
{
    char* header;
    char* player;
    char* mods;
    char* unlocks;
    char* mission_state;
    char* merged;
    char* merged2;

    header = fex_profile_save_header_to_string(&save->header);
    player = fex_profile_save_player_to_string(&save->player);
    mods = fex_profile_save_mods_to_string(&save->mods);
    unlocks = fex_profile_save_mod_unlocks_to_string(&save->mod_unlocks);
    mission_state = fex_profile_save_mission_state_to_string(&save->mission_state);

    merged = util_str_merge(header, player);
    merged2 = util_str_merge(merged, mods);
    free(merged);
    merged = util_str_merge(merged2, unlocks);
    free(merged2);
    merged2 = util_str_merge(merged, mission_state);
    free(merged);

    free(header);
    free(player);
    free(mods);
    free(unlocks);
    free(mission_state);

    return merged2;
}

void asset_fex_usb_save_decrypt(uint8_t* buf, size_t len)
{
	for (size_t a = len - 1; a > 0; --a) {
		buf[a] = (buf[a] ^ buf[a - 1]) + ((a * 1234567) >> 8);
	}
}

void asset_fex_usb_save_encrypt(uint8_t* buf, size_t len)
{
	for (size_t a = 1; a < len; ++a) {
		buf[a] = (buf[a] - ((a * 1234567) >> 8)) ^ buf[a - 1];
	}
}