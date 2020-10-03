#ifndef ASSET_NXA_USB_RANK_H
#define ASSET_NXA_USB_RANK_H

#include <stdint.h>
#include <stdlib.h>

enum asset_nxa_usb_rank_sizes {
	ASSET_NXA_USB_RANK_MAX_STAGES = 3,
	ASSET_NXA_USB_RANK_MAX_RANK_ENTRIES = 256,
	ASSET_NXA_USB_RANK_USB_SERIAL_LEN = 64,
};

struct asset_nxa_usb_rank_entry {
	int32_t game_mode;
	int32_t play_order;
	int32_t play_score;
	int32_t grade;
	int32_t mileage;
	float play_time;
	float kcal;
	float v02;
} __attribute__((__packed__));

struct asset_nxa_usb_rank {
	uint32_t adler32;
	char usb_serial[ASSET_NXA_USB_RANK_USB_SERIAL_LEN];
	uint32_t num_rankings;
    struct asset_nxa_usb_rank_entry
        entries[ASSET_NXA_USB_RANK_MAX_RANK_ENTRIES][ASSET_NXA_USB_RANK_MAX_STAGES];
} __attribute__((__packed__));

struct asset_nxa_usb_rank* asset_nxa_usb_rank_new(void);

// update checksum and prepare profile to get encrypted
void asset_nxa_usb_rank_finalize(struct asset_nxa_usb_rank* rank);

char* asset_nxa_usb_rank_to_string(const struct asset_nxa_usb_rank* rank);

void asset_nxa_usb_rank_decrypt(uint8_t* buf, size_t len);

void asset_nxa_usb_rank_encrypt(uint8_t* buf, size_t len);

#endif