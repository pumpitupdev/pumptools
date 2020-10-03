#include <string.h>

#include "asset/zero/lib/settings.h"

#include "util/adler32.h"
#include "util/mem.h"

struct asset_zero_settings* asset_zero_settings_new(void)
{
    struct asset_zero_settings* settings;

    settings = util_xmalloc(sizeof(struct asset_zero_settings));

    memset(settings->ff_pad, 0xFF, sizeof(settings->ff_pad));
    memcpy(settings->ident, "ZERO", 4);
    /* Updated using finalize */
    settings->adler32 = 0;
    settings->total_uptime = 0;
    settings->language = ASSET_ZERO_SETTINGS_LANG_ENG;
    memset(settings->song_play_count, 0, sizeof(uint32_t) * ASSET_ZERO_SETTINGS_MAX_SONGS);
    settings->game_option_default_station = ASSET_ZERO_SETTINGS_STATION_EASY;
    settings->game_mode = ASSET_ZERO_SETTINGS_GAME_MODE_NORMAL;
    settings->game_level = ASSET_ZERO_SETTINGS_LEVEL_NORMAL;
    settings->game_stage_break = ASSET_ZERO_SETTINGS_STAGE_BREAK_2ND;
    settings->game_demo_sound = 1;
    settings->game_show_help = 1;
    settings->game_mercy_ticket = 0;
    settings->game_score_per_ticket = 0;
    settings->coin_limit = 0;
    settings->coin1_setting = 1;
    settings->coin2_setting = 1;
    memset(settings->mission_unlock_flags, 0, ASSET_ZERO_SETTINGS_NUM_UNLOCK_FLAGS);
    settings->unkn2 = 0x2A;
    settings->unkn3 = 0;
    settings->bookkeeping_coin1_total = 0;
    settings->bookkeeping_coin2_total = 0;
    settings->bookkeeping_service_total = 0;
    settings->bookkeeping_ticket_1 = 0;
    settings->bookkeeping_ticket_2 = 0;
    settings->region = 0xFF;
    memset(settings->censor_table, 0, ASSET_ZERO_SETTINGS_MAX_SONGS);

    return settings;
}

void asset_zero_settings_finalize(struct asset_zero_settings* settings)
{
    /* Seems odd, but they just checksum the game settings part...*/
    settings->adler32 = util_adler32_calc(1, (const uint8_t*) &settings->game_mode, 8);
}

void asset_zero_settings_unlock_all(struct asset_zero_settings* settings)
{
    /* Mark all missions cleared to unlock all missions and content locked by some missions */
    memset(settings->mission_unlock_flags, 1, ASSET_ZERO_SETTINGS_NUM_UNLOCK_FLAGS);
}

char* asset_zero_settings_to_string(const struct asset_zero_settings* settings)
{
    // TODO
    return "";
}