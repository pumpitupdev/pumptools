#ifndef ASSET_FEX_USB_SAVE_H
#define ASSET_FEX_USB_SAVE_H

#include <stdint.h>
#include <stdlib.h>

enum asset_fex_usb_save_noteskins {
  ASSET_FEX_USB_SAVE_SKIN_DEFAULT = 0x00,
  ASSET_FEX_USB_SAVE_SKIN_HANAFUDA = 0x01,
  ASSET_FEX_USB_SAVE_SKIN_CLASSIC = 0x02,
  ASSET_FEX_USB_SAVE_SKIN_CANDY = 0x03,
  ASSET_FEX_USB_SAVE_SKIN_PORING = 0x04,
  ASSET_FEX_USB_SAVE_SKIN_MUSICAL_NOTES = 0x05,
  ASSET_FEX_USB_SAVE_SKIN_CANON_D = 0x06,
  ASSET_FEX_USB_SAVE_SKIN_PLAYING_CARDS = 0x07,
  ASSET_FEX_USB_SAVE_SKIN_NX = 0x08,
  ASSET_FEX_USB_SAVE_SKIN_DIGNITY = 0x09,
  ASSET_FEX_USB_SAVE_SKIN_HORSE_HEAD = 0x0A,
  ASSET_FEX_USB_SAVE_SKIN_DOG = 0x0B,
  ASSET_FEX_USB_SAVE_SKIN_GIRL = 0x0C,
  ASSET_FEX_USB_SAVE_SKIN_FIRE = 0x0D,
  ASSET_FEX_USB_SAVE_SKIN_ICE = 0x0E,
  ASSET_FEX_USB_SAVE_SKIN_WIND = 0x0F,
  ASSET_FEX_USB_SAVE_SKIN_RED_DP = 0x10,
  ASSET_FEX_USB_SAVE_SKIN_BLUE_DP = 0x11,
  ASSET_FEX_USB_SAVE_SKIN_YELLOW_DP = 0x12,
  ASSET_FEX_USB_SAVE_SKIN_NXA = 0x13,
  ASSET_FEX_USB_SAVE_SKIN_NX2 = 0x14,
  ASSET_FEX_USB_SAVE_SKIN_ELECTRIC_FIESTA = 0x15,
  ASSET_FEX_USB_SAVE_SKIN_DRUM = 0x16,
  ASSET_FEX_USB_SAVE_SKIN_BULLET = 0x17,
  ASSET_FEX_USB_SAVE_SKIN_BLUE_DRUM = 0x18,
  ASSET_FEX_USB_SAVE_SKIN_RED_DRUM = 0x19,
  ASSET_FEX_USB_SAVE_SKIN_YELLOW_DRUM = 0x1A,
  ASSET_FEX_USB_SAVE_SKIN_WORLD_CUP = 0x1B,
  ASSET_FEX_USB_SAVE_SKIN_PREX3 = 0x1C,
  ASSET_FEX_USB_SAVE_SKIN_FIESTA_BASIC = 0x1D,
};

struct asset_fex_usb_save_header {
  /* 0x00 */
  uint32_t adler32;
  /* 0x04: NULL terminated */
  char usb_serial[0x40];
  /* 0x44: Deprecated from old games */
  uint32_t dongle_serial;
} __attribute__((__packed__));

struct asset_fex_usb_save_player {
  /* 0x48 */
  uint32_t unkn;
  /* 0x4C */
  uint32_t unkn2;
  /* 0x50 */
  uint8_t avatar_id;
  /* 0x51: Range: 0x00 - 0x21 */
  uint8_t player_lvl;
  /* 0x52: PumBi sets it to 0xC4 */
  uint16_t unkn3;
  /* 0x54: NULL terminated */
  char player_id[8];
  /* 0x5C */
  uint32_t unkn4;
  /* 0x60 */
  uint32_t unkn5;
  /* 0x64: Each time you save, it inc. by 6... */
  uint32_t unkn6;
  /* 0x68 */
  uint32_t unkn7;
  /* 0x6C */
  uint32_t unkn8;
  /* 0x70 */
  uint32_t unkn9;
  /* 0x74 */
  uint32_t unkn10;
  /* 0x78 */
  uint32_t unkn11;
  /* 0x7C */
  uint32_t unkn12;
  /* 0x80 */
  uint32_t unkn13;
  /* 0x84 */
  uint32_t unkn14;
  /* 0x88 */
  uint32_t unkn15;
  /* 0x8C */
  uint32_t total_step;
  /* 0x90 */
  uint32_t play_count;
  /* 0x94 */
  uint32_t exp;
  /* 0x98 */
  uint32_t num_quest_world_passed;
  /* 0x9C */
  uint32_t unkn16;
  /* 0xA0 */
  uint32_t arcade_score;
  /* 0xA8 */
  uint32_t num_battle_wins;
  /* 0xAC */
  uint32_t num_battle_loss;
  /* 0xB0 */
  uint32_t num_battle_draw;
} __attribute__((__packed__));

struct asset_fex_usb_save_player_mods {
  /* 0xB4: The first set of saved modifiers (see MOD_1 below) */
  uint32_t modifiers;
  /* 0xB8: If the +0.5x mod is enabled (boolean) */
  uint32_t half_speed_mod;
  /* 0xBC: Which noteskin is currently selected (see NOTE_S below) */
  uint32_t noteskin;
} __attribute__((__packed__));

struct asset_fex_usb_save_mod_unlocks {
  /* 0xC0: Unknown Unlocks Area */
  uint8_t unkn0[0x12];
  /* 0xD2: Boolean for unlocking VA Mod. */
  uint8_t display_va;
  /* 0xD3: Unknown Unlocks Area */
  uint8_t unkn1[0x0D];
  /* 0xE0: Array of bools - Which Noteskins are Unlocked. */
  uint8_t noteskins[0x10];
  /* 0xF0: Unknown Unlocks Area */
  uint8_t unkn2[0x04];
  /* 0xF4: Array of bools - Which Path Mods are Unlocked. */
  uint8_t path[0x03];
  /* 0xF7: Unknown Unlocks Area */
  uint8_t unkn3[0x06];
  /* 0xFE: Boolean for unlocking XJ */
  uint8_t xj;
  /* 0x100: Array of bools - Which System Mods are Unlocked. */
  uint8_t system[0x02];
  /* 0x102: Array of bools - Which Rush/NOT Modes are Unlocked. */
  uint8_t rush[0x08];
  /* 0x10A: Unknown Unlocks Area */
  uint8_t unkn4[0x1A];
} __attribute__((__packed__));

struct asset_fex_usb_save_mission_state {
  /* 0x124 */
  uint32_t quest_world_location;
  /* 0x128 */
  uint32_t skill_up_zone_location;
} __attribute__((__packed__));

struct asset_fex_usb_save {
  struct asset_fex_usb_save_header header;
  struct asset_fex_usb_save_player player;
  struct asset_fex_usb_save_player_mods mods;
  struct asset_fex_usb_save_mod_unlocks mod_unlocks;
  struct asset_fex_usb_save_mission_state mission_state;

  /* Not verified, yet. just notes from old tools */

  /* 0x17AD (0x0A): Quest world inventory */
  /* 0x17B7: ??? */
  /* 0x17B8 (0x1C): Quest world warp unlocks */

  /* 0x17AC (0x11): QW inventory */

  /* 0x17D4 or 0x17D5 ? (0x14): favorites channel */
  /* 0x2E48 - 0x1A5EF: step chart blacklist */
  /* 0x2E48 - 0x1A5EF: step chart unlocks = blacklist */

  /* pumbi stepcharts */
  /* 0x6047: pumbi stepchart 1009 Level S15 */
  /* 0x6083 */
  /* 0x60AB */
  /* 0x647F */
  /* 0x64BB */
  /* 0x64CF */
  /* 0x650B */
  /* 0x6533 */
  /* 0x655B */
  /* 0x65AB */
  /* 0x660F */
} __attribute__((__packed__));

struct asset_fex_usb_save *asset_fex_usb_save_new(void);

// update checksum and prepare profile to get encrypted
void asset_fex_usb_save_finalize(struct asset_fex_usb_save *save);

char *asset_fex_usb_save_to_string(const struct asset_fex_usb_save *save);

void asset_fex_usb_save_decrypt(uint8_t *buf, size_t len);

void asset_fex_usb_save_encrypt(uint8_t *buf, size_t len);

#endif
