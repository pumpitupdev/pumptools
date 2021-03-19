#ifndef ASSET_F2_USB_SAVE_H
#define ASSET_F2_USB_SAVE_H

#include <stdint.h>
#include <stdlib.h>

struct asset_f2_usb_save_machine_stats {
  uint32_t beginner_mode_play_count;
  uint32_t maniac_mode_play_count;
  uint32_t unkn;
  uint32_t unkn2;
  uint32_t battle_mode_play_count;
  uint32_t mission_zone_play_count;
  uint32_t skill_zone_play_count;
  uint32_t coop_mission_play_count;
} __attribute__((__packed__));

struct asset_f2_usb_save_machine_info {
  /* NULL terminated */
  char version[8];
  /* NULL terminated */
  char cpu[128];
  /* NULL terminated */
  char motherboard[128];
  /* NULL terminated */
  char gfxcard[128];
  /* NULL terminated */
  char hdd[32];
  uint32_t total_ram_bytes;
  uint32_t haspkey_id;
} __attribute__((__packed__));

struct asset_f2_usb_save_header {
  uint32_t adler32;
  uint32_t adler_seed;
  /* NULL terminated */
  char player_id[8];
  uint32_t region;
  uint32_t avatar_id;
  uint32_t level;
  float total_calories;
  float total_v02;
  uint64_t num_running_steps;
  uint64_t num_games_played;
  uint64_t exp;
  uint64_t arcade_score;
  uint32_t num_missions_completed;
  uint32_t num_coop_missions_completed;
  uint32_t battle_count_wins;
  uint32_t battle_count_loses;
  uint32_t battle_count_draws;
  /* TODO: modifier stuff? */
  uint64_t unlock;
  uint32_t unlock_seed;
} __attribute__((__packed__));

struct asset_f2_usb_save_machine_highscore_entry {
  uint32_t song_id;
  uint8_t difficulty;
  uint8_t mode;
  uint8_t grade;
  uint8_t clear_state;
  uint32_t score_total;
  uint32_t play_count;
  uint32_t unkn;
  /* NULL terminated */
  char player_id[8];
  uint32_t unkn2;
  uint32_t null_padding;
} __attribute__((__packed__));

struct asset_f2_usb_save_highscore_entry {
  uint32_t song_id;
  uint8_t difficulty;
  uint8_t mode;
  uint8_t grade;
  uint8_t clear_and_unlock_state;
  uint32_t score_total;
  uint32_t play_count;
  uint32_t null_padding;
} __attribute__((__packed__));

struct asset_f2_usb_save {
  /* start offset: 0x00 */
  struct asset_f2_usb_save_header header;
  /* start offset: 0x120 */
  struct asset_f2_usb_save_machine_info machine_info;
  struct asset_f2_usb_save_machine_stats machine_stats;
  /* start offset: 0x2F0 */

  // TODO continue
} __attribute__((__packed__));

struct asset_f2_usb_save *asset_f2_usb_save_new(void);

// update checksum and prepare profile to get encrypted
void asset_f2_usb_save_finalize(struct asset_f2_usb_save *save);

char *asset_f2_usb_save_to_string(const struct asset_f2_usb_save *save);

void asset_f2_usb_save_decrypt(uint8_t *buf, size_t len);

void asset_f2_usb_save_encrypt(uint8_t *buf, size_t len);

#endif