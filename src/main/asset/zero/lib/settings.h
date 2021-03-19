#ifndef ASSET_ZERO_SETTINGS_H
#define ASSET_ZERO_SETTINGS_H

#include <stdint.h>

#define ASSET_ZERO_SETTINGS_FILE_NAME "PIUZERO.INI"
#define ASSET_ZERO_SETTINGS_MAX_SONGS 200
#define ASSET_ZERO_SETTINGS_NUM_UNLOCK_FLAGS 59

enum asset_zero_settings_language {
  ASSET_ZERO_SETTINGS_LANG_KOR = 0,
  ASSET_ZERO_SETTINGS_LANG_ENG = 1,
  ASSET_ZERO_SETTINGS_LANG_SP = 2,
  ASSET_ZERO_SETTINGS_LANG_CH = 3,
};

enum asset_zero_settings_station {
  ASSET_ZERO_SETTINGS_STATION_EASY = 0,
  ASSET_ZERO_SETTINGS_STATION_ARCADE = 1,
  ASSET_ZERO_SETTINGS_STATION_REMIX = 2,
};

enum asset_zero_settings_game_mode {
  ASSET_ZERO_SETTINGS_GAME_MODE_NORMAL = 0,
  ASSET_ZERO_SETTINGS_GAME_MODE_EVENT = 1,
};

enum asset_zero_settings_level {
  ASSET_ZERO_SETTINGS_LEVEL_EASY = 0,
  ASSET_ZERO_SETTINGS_LEVEL_NORMAL = 1,
  ASSET_ZERO_SETTINGS_LEVEL_HARD = 2,
};

enum asset_zero_settings_stage_break {
  ASSET_ZERO_SETTINGS_STAGE_BREAK_OFF = 0,
  ASSET_ZERO_SETTINGS_STAGE_BREAK_1ST = 1,
  ASSET_ZERO_SETTINGS_STAGE_BREAK_2ND = 2,
  ASSET_ZERO_SETTINGS_STAGE_BREAK_3RD = 3,
  ASSET_ZERO_SETTINGS_STAGE_BREAK_4TH = 4,
};

struct asset_zero_settings {
  /* Padding to fill up to 4k EEPROM legacy size */
  uint8_t ff_pad[0xBA4];
  /* ZERO */
  char ident[4];
  uint32_t adler32;
  uint32_t total_uptime;
  uint8_t language;
  uint32_t song_play_count[ASSET_ZERO_SETTINGS_MAX_SONGS];
  uint32_t game_option_default_station;
  uint8_t game_mode;
  uint8_t game_level;
  uint8_t game_stage_break;
  /* 0 off, 1 on */
  uint8_t game_demo_sound;
  /* 0 off, 1 on */
  uint8_t game_show_help;
  /* 0 off, 1 on */
  uint8_t game_mercy_ticket;
  /* 0 off, 1 - 6 for score */
  uint8_t game_score_per_ticket;
  /* 0 none */
  uint8_t coin_limit; // 0 none
  /* 0 freeplay */
  uint8_t coin1_setting;
  /* 0 freeplay */
  uint8_t coin2_setting;
  /* Unlock flags for missions (total of 30) */
  uint8_t mission_unlock_flags[ASSET_ZERO_SETTINGS_NUM_UNLOCK_FLAGS];
  /* Default value 0x2A */
  uint32_t unkn2;
  uint32_t unkn3;
  uint32_t bookkeeping_coin1_total;
  uint32_t bookkeeping_coin2_total;
  uint32_t bookkeeping_service_total;
  uint32_t bookkeeping_ticket_1;
  uint32_t bookkeeping_ticket_2;
  /* FF -> not used, censoring globally disabled. unused feature though */
  uint8_t region;
  /* 0 = censoring of song off */
  uint8_t censor_table[ASSET_ZERO_SETTINGS_MAX_SONGS];
} __attribute__((__packed__));

struct asset_zero_settings *asset_zero_settings_new(void);

// update checksum
void asset_zero_settings_finalize(struct asset_zero_settings *settings);

// sets all missions cleared to unlock special stage songs and other missions
void asset_zero_settings_unlock_all(struct asset_zero_settings *settings);

char *asset_zero_settings_to_string(const struct asset_zero_settings *settings);

#endif
