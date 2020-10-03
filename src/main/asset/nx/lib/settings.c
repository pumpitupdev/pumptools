#include <string.h>

#include "asset/nx/lib/settings.h"

#include "util/adler32.h"
#include "util/mem.h"

struct asset_nx_settings* asset_nx_settings_new(void)
{
    struct asset_nx_settings* settings;

    settings = util_xmalloc(sizeof(struct asset_nx_settings));

    memset(settings->ff_pad, 0xFF, sizeof(settings->ff_pad));
    memcpy(settings->ident, "NX10", 4);
    /* Updated using finalize */
    settings->adler32 = 0;
    settings->total_uptime = 0;
    settings->language = ASSET_NX_SETTINGS_LANG_ENG;
    memset(settings->song_play_count, 0, sizeof(uint32_t) * ASSET_NX_SETTINGS_MAX_SONGS);
    settings->game_option_default_station = ASSET_NX_SETTINGS_STATION_TRAIN;
    settings->game_mode = ASSET_NX_SETTINGS_GAME_MODE_NORMAL;
    settings->game_level = ASSET_NX_SETTINGS_LEVEL_NORMAL;
    settings->game_stage_break = ASSET_NX_SETTINGS_STAGE_BREAK_2ND;
    settings->game_demo_sound = 1;
    settings->game_show_help = 1;
    settings->game_mercy_ticket = 0;
    settings->game_score_per_ticket = 0;
    settings->coin_limit = 0;
    settings->coin1_setting = 1;
    settings->coin2_setting = 1;
    memset(settings->unknown_unlock_flags, 0, ASSET_NX_SETTINGS_NUM_UNLOCK_FLAGS);
    settings->unkn2 = 0x2A;
    settings->unkn3 = 0;
    settings->bookkeeping_coin1_total = 0;
    settings->bookkeeping_coin2_total = 0;
    settings->bookkeeping_service_total = 0;
    settings->bookkeeping_play_total = 0;
    settings->bookkeeping_faep21_play_count = 0;
    settings->bookkeeping_ticket_1 = 0;
    settings->bookkeeping_ticket_2 = 0;
    settings->region = 0xFF;
    memset(settings->censor_table, 0, ASSET_NX_SETTINGS_MAX_SONGS);

    return settings;
}

void asset_nx_settings_finalize(struct asset_nx_settings* settings)
{
    /* Seems odd, but they just checksum the game settings part...*/
    settings->adler32 = util_adler32_calc(1, (const uint8_t*) &settings->game_mode, 8);
}

void asset_nx_settings_unlock_all(struct asset_nx_settings* settings)
{
    /* FAEP 2-1 must be played at least 100 times to unlock FAEP 2-2 */
    settings->bookkeeping_faep21_play_count = 100;

    /* To unlock all world tour missions and special zone songs, set this to 20k.
       The first 10k unlock only the first half of the missions while another 10k unlocks the second half (wtf) */
    settings->bookkeeping_play_total = 20000;
}

char* asset_nx_settings_to_string(const struct asset_nx_settings* settings)
{
    // TODO
    return "";
}