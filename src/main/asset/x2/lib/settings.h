#ifndef ASSET_X2_SETTINGS_H
#define ASSET_X2_SETTINGS_H

#include <stdint.h>

#define ASSET_X2_SETTINGS_FILE_NAME "PIUEXCEED.INI"
#define ASSET_X2_SETTINGS_MAX_SONGS 139
#define ASSET_X2_SETTINGS_HIGH_SCORE_COUNT 20
#define ASSET_X2_SETTINGS_HIGH_SCORE_NAME_LEN 4

enum asset_x2_settings_language {
  ASSET_X2_SETTINGS_LANG_KOR = 0,
  ASSET_X2_SETTINGS_LANG_ENG = 1,
  ASSET_X2_SETTINGS_LANG_SP = 2,
  ASSET_X2_SETTINGS_LANG_CH = 3,
};

enum asset_x2_settings_game_mode {
  ASSET_X2_SETTINGS_GAME_MODE_NORMAL = 0,
  ASSET_X2_SETTINGS_GAME_MODE_EVENT = 1,
};

enum asset_x2_settings_level {
  ASSET_X2_SETTINGS_LEVEL_EASY = 0,
  ASSET_X2_SETTINGS_LEVEL_NORMAL = 1,
  ASSET_X2_SETTINGS_LEVEL_HARD = 2,
};

enum asset_x2_settings_stage_break {
  ASSET_X2_SETTINGS_STAGE_BREAK_OFF = 0,
  ASSET_X2_SETTINGS_STAGE_BREAK_1ST = 1,
  ASSET_X2_SETTINGS_STAGE_BREAK_2ND = 2,
  ASSET_X2_SETTINGS_STAGE_BREAK_3RD = 3,
  ASSET_X2_SETTINGS_STAGE_BREAK_4TH = 4,
};

struct asset_x2_settings {
  /* Padding to fill up to 4k EEPROM legacy size */
  uint8_t ff_pad[0x51B];
  uint8_t unkn[10];
  uint8_t unlock_canond_full;
  uint32_t total_uptime;
  uint8_t unkn69;
  uint8_t song_blacklist[ASSET_X2_SETTINGS_MAX_SONGS];
  uint8_t unkn3[0x75];
  /* EX02 */
  char ident[4];
  uint32_t adler32;
  uint16_t song_play_count[ASSET_X2_SETTINGS_MAX_SONGS];
  uint16_t padding;
  uint32_t high_score_scores[ASSET_X2_SETTINGS_HIGH_SCORE_COUNT];
  /* 20 names a 4 chars for the default high score list */
  char high_score_names[ASSET_X2_SETTINGS_HIGH_SCORE_COUNT]
                       [ASSET_X2_SETTINGS_HIGH_SCORE_NAME_LEN];
  /* 0 none */
  uint8_t coin_limit;
  uint8_t game_mode;
  uint8_t game_level;
  uint8_t game_stage_break;
  uint8_t language;
  /* 0 off, 1 on */
  uint8_t game_demo_sound;
  /* 0 off, 1 on */
  uint8_t game_show_help;
  /* 0 freeplay */
  uint8_t coin1_setting;
  /* 0 freeplay */
  uint8_t coin2_setting;
  uint32_t bookkeeping_coin1_total;
  uint32_t bookkeeping_coin2_total;
  uint32_t bookkeeping_service_total;
} __attribute__((__packed__));

struct asset_x2_settings *asset_x2_settings_new(void);

// update checksum
void asset_x2_settings_finalize(struct asset_x2_settings *settings);

void asset_x2_settings_unlock_all(struct asset_x2_settings *settings);

char *asset_x2_settings_to_string(const struct asset_x2_settings *settings);

#endif
