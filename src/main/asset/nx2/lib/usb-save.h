#ifndef ASSET_NX2_USB_SAVE_H
#define ASSET_NX2_USB_SAVE_H

#include <stdint.h>
#include <stdlib.h>

#define ASSET_NX2_USB_SAVE_SIZE 30780

enum asset_nx2_usb_save_limits {
	ASSET_NX2_USB_SAVE_SERIAL_ID_MAX = 64,
    ASSET_NX2_USB_SAVE_PLAYER_ID_MAX = 12,
	ASSET_NX2_USB_SAVE_WORLDMAX_LOC_MAX = 128,
	ASSET_NX2_USB_SAVE_SONG_MAX = 256,
	ASSET_NX2_USB_SAVE_MISSION_MAX = 1024,
	ASSET_NX2_USB_SAVE_BARRICADE_MAX = 16,
	ASSET_NX2_USB_SAVE_EVENT_MAX = 128,
	ASSET_NX2_USB_SAVE_WARP_MAX = 8,
	ASSET_NX2_USB_SAVE_NUM_MODES = 5,
};

struct asset_nx2_usb_save_time {
	int16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t min;
	uint16_t ms;
} __attribute__((__packed__));

struct asset_nx2_usb_save_review {
	char player_id[ASSET_NX2_USB_SAVE_PLAYER_ID_MAX];
	int32_t mileage;
	int32_t reward_count;
	int32_t worldmax_count;
	int32_t play_count;
	char worldmax_current_land[ASSET_NX2_USB_SAVE_WORLDMAX_LOC_MAX];
	char worldmax_current_mission[ASSET_NX2_USB_SAVE_WORLDMAX_LOC_MAX];
	int32_t kcal;
	int32_t v02;
} __attribute__((__packed__));

struct asset_nx2_usb_save_stats {
	uint32_t adler32;
	char usb_serial[ASSET_NX2_USB_SAVE_SERIAL_ID_MAX];
	struct asset_nx2_usb_save_time timestamp;
	int32_t avatar_id;
	int32_t rank;
	int32_t country_id;
	char player_id[ASSET_NX2_USB_SAVE_PLAYER_ID_MAX];
	int32_t mileage;
	int32_t play_count;
	int32_t kcal;
	int32_t v02;
	int32_t worldmax_map_position;
	int32_t reward_count;
	int32_t worldmax_count;

	struct {
		/* bits 0-4 for modes/stepcharts */
		uint8_t mode : 5;
		/* unused bit */
		uint8_t unused : 1;
		/* bit 6 song itself */
		uint8_t song : 1;
		/* bit 7 reveal flag */
		uint8_t reveal : 1;
	} song_unlocks[ASSET_NX2_USB_SAVE_SONG_MAX] __attribute__((__packed__));

	struct {
		uint8_t clear_flags_directions : 4;
		uint8_t clear_flag_mission : 1;
		uint8_t unused : 3;
	} worldmax_mission_unlocks[ASSET_NX2_USB_SAVE_MISSION_MAX]
		__attribute__((__packed__));

	int32_t worldmax_high_score[ASSET_NX2_USB_SAVE_MISSION_MAX];
	int32_t worldmax_challenge[ASSET_NX2_USB_SAVE_MISSION_MAX];

	struct {
		uint8_t flag_open : 1;
		uint8_t unused : 7;
	} worldmax_barricade[ASSET_NX2_USB_SAVE_BARRICADE_MAX]
		__attribute__((__packed__));

	struct {
		uint8_t flag_cleared : 1;
		uint8_t unused : 7;
	} worldmax_event[ASSET_NX2_USB_SAVE_EVENT_MAX]  __attribute__((__packed__));

	struct {
		uint8_t direction_clear : 4;
		uint8_t clear : 1;
		uint8_t unused : 3;
	} worldmax_warp[ASSET_NX2_USB_SAVE_WARP_MAX]  __attribute__((__packed__));

	char worldmax_current_land[ASSET_NX2_USB_SAVE_WORLDMAX_LOC_MAX];
	char worldmax_current_mission[ASSET_NX2_USB_SAVE_WORLDMAX_LOC_MAX];

	struct {
		int32_t score;
		char player_id[ASSET_NX2_USB_SAVE_PLAYER_ID_MAX];
	} song_scores[ASSET_NX2_USB_SAVE_SONG_MAX][ASSET_NX2_USB_SAVE_NUM_MODES]
		__attribute__((__packed__));
} __attribute__((__packed__));

struct asset_nx2_usb_save {
    struct asset_nx2_usb_save_review review;
    struct asset_nx2_usb_save_stats stats;
} __attribute__((__packed__));

struct asset_nx2_usb_save* asset_nx2_usb_save_new(void);

// update checksum and prepare profile to get encrypted
void asset_nx2_usb_save_finalize(struct asset_nx2_usb_save* save);

char* asset_nx2_usb_save_to_string(const struct asset_nx2_usb_save* save);

void asset_nx2_usb_save_decrypt(uint8_t* buf, size_t len);

void asset_nx2_usb_save_encrypt(uint8_t* buf, size_t len);

#endif