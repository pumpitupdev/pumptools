#include <string.h>

#include "asset/x2/lib/settings.h"

#include "util/adler32.h"
#include "util/mem.h"

static const char* _asset_x2_settings_defaut_high_score_names[] = {
    "MKII",
    "YAHP",
    "KANN",
    "ZINY",
    "PIAH",
    "AHN ",
    "PIYO",
    "HIKO",
    "HALO",
    "BAJI",
    "YUTE",
    "SS  ",
    "SIHO",
    "AZKI",
    "JUNE",
    "GOON",
    "ARI*",
    "TAMA",
    "YSMA",
    "PIU "
};

struct asset_x2_settings* asset_x2_settings_new(void)
{
    struct asset_x2_settings* settings;

    settings = util_xmalloc(sizeof(struct asset_x2_settings));

    memset(settings->ff_pad, 0xFF, sizeof(settings->ff_pad));
    memset(settings->unkn, 0, sizeof(settings->unkn));
    settings->unlock_canond_full = 0;
    settings->total_uptime = 0;
    settings->unkn69 = 0x69;
    memset(settings->song_blacklist, 0, ASSET_X2_SETTINGS_MAX_SONGS);
    memset(settings->unkn3, 0, sizeof(settings->unkn3));
    memcpy(settings->ident, "EX02", 4);
    /* Updated using finalize */
    settings->adler32 = 0;
    memset(settings->song_play_count, 0, sizeof(uint16_t) * ASSET_X2_SETTINGS_MAX_SONGS);
    settings->padding = 0;

    for (uint8_t i = 0; i < ASSET_X2_SETTINGS_HIGH_SCORE_COUNT; i++) {
        settings->high_score_scores[i] = 10000u - (500u * i);
        memcpy(&settings->high_score_names[i], &_asset_x2_settings_defaut_high_score_names[i],
            ASSET_X2_SETTINGS_HIGH_SCORE_NAME_LEN);
    }

    settings->coin_limit = 0;
    settings->game_mode = ASSET_X2_SETTINGS_GAME_MODE_NORMAL;
    settings->game_level = ASSET_X2_SETTINGS_LEVEL_NORMAL;
    settings->game_stage_break = ASSET_X2_SETTINGS_STAGE_BREAK_2ND;
    settings->language = ASSET_X2_SETTINGS_LANG_ENG;
    settings->game_demo_sound = 1;
    settings->game_show_help = 1;
    settings->coin1_setting = 1;
    settings->coin2_setting = 1;
    settings->bookkeeping_coin1_total = 0;
    settings->bookkeeping_coin2_total = 0;
    settings->bookkeeping_service_total = 0;

    return settings;
}

void asset_x2_settings_finalize(struct asset_x2_settings* settings)
{
    /* Seems odd, but they just checksum the game settings part...*/
    settings->adler32 = util_adler32_calc(1, (const uint8_t*) &settings->game_mode, 8);
}

void asset_x2_settings_unlock_all(struct asset_x2_settings* settings)
{
    settings->unlock_canond_full = 1;
}

char* asset_x2_settings_to_string(const struct asset_x2_settings* settings)
{
    // TODO
    return "";
}