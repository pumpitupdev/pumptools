#ifndef ASSET_NXA_USB_SAVE_H
#define ASSET_NXA_USB_SAVE_H

#include <stdint.h>
#include <stdlib.h>

enum asset_nxa_usb_save_limits {
	ASSET_NXA_USB_SAVE_SERIAL_ID_MAX = 64,
    ASSET_NXA_USB_SAVE_PLAYER_ID_MAX = 12,
	ASSET_NXA_USB_SAVE_WORLDMAX_LOC_MAX = 128,
	ASSET_NXA_USB_SAVE_SONG_MAX = 256,
	ASSET_NXA_USB_SAVE_MISSION_MAX = 1024,
	ASSET_NXA_USB_SAVE_BARRICADE_MAX = 16,
	ASSET_NXA_USB_SAVE_EVENT_MAX = 128,
	ASSET_NXA_USB_SAVE_WARP_MAX = 8,
	ASSET_NXA_USB_SAVE_NUM_MODES = 5,
};

struct asset_nxa_usb_save_time {
	int16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t min;
	uint16_t ms;
} __attribute__((__packed__));

/* Not encrypted and read only, offset 0x00 to 0x144 (excl) */
struct asset_nxa_usb_save_review {
	/* 0x00, 12 byte char array: max char length is 8 which gets displayed
	   ingame */
	char player_id[ASSET_NXA_USB_SAVE_PLAYER_ID_MAX];
	/* 0x0C, 4 byte int */
	uint32_t mileage;
	/* 0x10, 4 byte int: achieved reward count, max 65 */
	uint32_t reward_count;
	/* 0x14, 4 byte int: successful mission count, max 634 */
	uint32_t worldmax_count;
	/* 0x18, 4 byte int */
	uint32_t play_count;
	/* 0x1C, 128 byte char array */
	char worldmax_current_land[ASSET_NXA_USB_SAVE_WORLDMAX_LOC_MAX];
	/* 0x9C, 128 byte char array */
	char worldmax_current_mission[ASSET_NXA_USB_SAVE_WORLDMAX_LOC_MAX];
	/* 0x11C, 4 byte int */
	uint32_t kcal;
	/* 0x120, 4 byte int */
	uint32_t v02;
	/* 0x124, 8 byte */
	struct asset_nxa_usb_save_time timestamp;
	/* 0x12C, 4 byte int */
	uint32_t running_step;
	/* 0x130, 4 byte float */
	float play_time_minutes;
	/* 0x134, 4 byte float */
	float total_completion_percentage;
	/* 0x138, 4 byte float */
	float arcade_percentage;
	/* 0x13C, 4 byte float */
	float brain_shower_percentage;
	/* 0x140, 4 byte float */
	float special_percentage;
} __attribute__((__packed__));

struct asset_nxa_usb_save_stats {
	/* 0x144, 4 byte int */
	uint32_t adler32;
	/* 0x148, 64 byte */
	char usb_serial[ASSET_NXA_USB_SAVE_SERIAL_ID_MAX];
	/* 0x188, 8 byte */
	struct asset_nxa_usb_save_time timestamp;
	/* 0x190, 4 byte int */
	int32_t avatar_id;
	/* 0x194, 4 byte int: setting this non null will give you a trophy */
	int32_t rank;
	/* 0x198, 4 byte int: seems unused */
	int32_t country_id;
	/* 0x19C, 12 byte char array */
	char player_id[ASSET_NXA_USB_SAVE_PLAYER_ID_MAX];
	/* 0x1A8, 4 byte int */
	int32_t mileage;
	/* 0x1AD, 4 byte int */
	int32_t play_count;
	/* 0x1B0, 4 byte int */
	int32_t kcal;
	/* 0x1B4, 4 byte int */
	int32_t v02;
	/* 0x1B8, 4 byte int */
	int32_t worldmax_map_position;
	/* 0x1BC, 4 byte int: achieved reward count, max 65 */
	int32_t reward_count;
	/* 0x1C0, 4 byte int: successful mission count, max 634 */
	int32_t worldmax_count;

	/* 0x1C4, 256 bytes */
	struct {
		/* bits 0-4 for modes/stepcharts */
		uint8_t mode : 5;
		/* unused bit */
		uint8_t unused : 1;
		/* bit 6 song itself */
		uint8_t song : 1;
		/* bit 7 reveal flag */
		uint8_t reveal : 1;
	} song_unlocks[ASSET_NXA_USB_SAVE_SONG_MAX] __attribute__((__packed__));

	/* 0x2C4, 1024 bytes */
	struct {
		uint8_t clear_flags_directions : 4;
		uint8_t clear_flag_mission : 1;
		uint8_t unused : 3;
	} worldmax_mission_unlocks[ASSET_NXA_USB_SAVE_MISSION_MAX]
		__attribute__((__packed__));

	/* 0x6C4, 4 byte int * 1024: high score for each mission */
	int32_t worldmax_high_score[ASSET_NXA_USB_SAVE_MISSION_MAX];

	/* 0x16C4, 4 byte int * 1024 */
	int32_t worldmax_challenge[ASSET_NXA_USB_SAVE_MISSION_MAX];

	/* 0x26C4, 16 bytes
	   0x00 for barricade clear and 0x01 for block */
	struct {
		uint8_t flag_open : 1;
		uint8_t unused : 7;
	} worldmax_barricade[ASSET_NXA_USB_SAVE_BARRICADE_MAX]
		__attribute__((__packed__));

	/* 0x26D4, 128 bytes
	   0x00 for event clear and 0x01 for not cleared */
	struct {
		uint8_t flag_cleared : 1;
		uint8_t unused : 7;
	} worldmax_event[ASSET_NXA_USB_SAVE_EVENT_MAX]  __attribute__((__packed__));

	/* 0x2754, 8 bytes
	   bit 0~3: clear bit for each direction bit:4 clear */
	struct {
		uint8_t direction_clear : 4;
		uint8_t clear : 1;
		uint8_t unused : 3;
	} worldmax_warp[ASSET_NXA_USB_SAVE_WARP_MAX]  __attribute__((__packed__));

	/* 0x275C, 128 byte char array */
	char worldmax_current_land[ASSET_NXA_USB_SAVE_WORLDMAX_LOC_MAX];

	/* 0x27DC, 128 byte char array */
	char worldmax_current_mission[ASSET_NXA_USB_SAVE_WORLDMAX_LOC_MAX];

	/* 0x285C */
	struct {
		int32_t score;
		char player_id[ASSET_NXA_USB_SAVE_PLAYER_ID_MAX];
	} song_scores[ASSET_NXA_USB_SAVE_SONG_MAX][ASSET_NXA_USB_SAVE_NUM_MODES]
		__attribute__((__packed__));

	/* 0x785C, 4 byte int */
	uint32_t security_dongle_mfgid;

	/* 0x7860, 4 byte int: <=3 Tells game to unlock new song */
	uint32_t unlock_signal;

	/* 0x7864, 4 byte int */
	uint32_t running_steps;

	/* 0x7868, 4 byte float */
	float total_play_time_min;

	/* 0x786C, 4 byte float */
	float total_completion_percentage;

	/* 0x7870, 4 byte float */
	float arcade_completion_percentage;

	/* 0x7874, 4 byte float */
	float brain_shower_completion_percentage;

	/* 0x7878, 4 byte float */
	float special_stage_completion_percentage;

	/* 0x787C TODO unknown
	   0x787C, 1 byte char * 4096 - Used by GetSongIndex -
	   Assuming Maps Scores to Song IDs */
	uint8_t unkn_some_map_scores_to_song_ids[4096];

	/* 0x887C TODO unknown
	   0x887C, 1 byte char * 576 - Song Locks/Unlocks - 0x40 Unlocks (Special Zone, etc.)
	   0x8A70 , 1 byte char * 36 - WorldMax Keys and various other events for completion
	   0x8A9C, 8 byte char* - A Worldmax ID (Perhaps one to mark progress in Space)
	   0x8AA4, 1 byte char * 24 - More WorldMax Event Switches */
	uint8_t unkn2[576];

	/* 0x8ABC, 4 byte int: possible values
		0x1 - grey
		0x2 - red
		0x3 - yellow
		0x4 - green
		0x5 - Dark Green
		0x6 - aqua
		0x7 - blue
		0x8 - purple
		0x9 - pink
		0xA - light grey
		0xB - Black
		0xC - Orange
	*/
	uint32_t break_item_slot;

	/* 0x8AC0, 4 byte int
		0x1 - Shield
		0x2 - PermaShield
		0x3 - Line Searcher
		0x4 - Perma Line Searcher
		0x5 - Life Up
		0x6 - Perma Life Up
		0x7 - Lucky
		0x8 - Perma Lucky
		0x9 - Portal Pass
	*/
	uint32_t func_item_slot;

	/* 0x8AC4, 4 byte int
		0x1 - BGA OFF
		0x2 - Perma BGA OFF
	*/
	uint32_t bga_item_slot;

	/* 0x8AC4, 4 byte int
		0x1 - 10+
		0x2 - 20+
		0x3 - 30+
	*/
	uint32_t time_item_slot;

	/* 0x8AC8, 4 byte int */
	uint32_t unknown;

	/* 0x8ACC, unknown stuff: More WorldMax Event Switches (Unused)? */
	uint8_t unknown2[12];

	/* 0x8AD8, 4 byte float - Unknown What this Does (Unused) */
	float unknown3;

	/* 0x8ADC, totally unknown stuff, just nulls */
	uint8_t unknown4[0x7F8];

	/* 0x92D4 */
	uint8_t unknown5[0xD01C];

	/* TODO missing switches for dark/light world stuff
	   0x8ACC, 1 byte char - More WorldMax Event Switches (Unused)
	   0x8AD8 , 4 byte float - Unknown What this Does (Unused)
	   0x92D4, High Score Table [See Below]
	   0x132D0, 5216 bytes - Unknown Table (Unused)
	   0x162D0, 24 bytes - Unknown Footer (Unused by Game - Most Likely PumBI)
	*/
} __attribute__((__packed__));

struct asset_nxa_usb_save {
    struct asset_nxa_usb_save_review review;
    struct asset_nxa_usb_save_stats stats;
} __attribute__((__packed__));

struct asset_nxa_usb_save* asset_nxa_usb_save_new(void);

/* update checksum and prepare profile to get encrypted */
void asset_nxa_usb_save_finalize(struct asset_nxa_usb_save* save);

char* asset_nxa_usb_save_to_string(const struct asset_nxa_usb_save* save);

void asset_nxa_usb_save_decrypt(uint8_t* buf, size_t len);

void asset_nxa_usb_save_encrypt(uint8_t* buf, size_t len);

#endif