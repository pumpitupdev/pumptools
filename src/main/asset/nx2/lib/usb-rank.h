#ifndef ASSET_NX2_USB_RANK_H
#define ASSET_NX2_USB_RANK_H

#include <stdint.h>
#include <stdlib.h>

#define ASSET_NX2_USB_RANK_SIZE 12296

enum asset_nx2_usb_rank_sizes {
  ASSET_NX2_USB_RANK_MAX_STAGES = 3,
  ASSET_NX2_USB_RANK_MAX_RANK_ENTRIES = 128,
};

struct asset_nx2_usb_rank_entry {
  int32_t game_mode;
  int32_t play_order;
  int32_t play_score;
  int32_t grade;
  int32_t mileage;
  float play_time;
  float kcal;
  float v02;
} __attribute__((__packed__));

struct asset_nx2_usb_rank {
  uint32_t adler32;
  uint32_t num_rankings;
  struct asset_nx2_usb_rank_entry entries[ASSET_NX2_USB_RANK_MAX_RANK_ENTRIES]
                                         [ASSET_NX2_USB_RANK_MAX_STAGES];
} __attribute__((__packed__));

struct asset_nx2_usb_rank *asset_nx2_usb_rank_new(void);

// update checksum and prepare profile to get encrypted
void asset_nx2_usb_rank_finalize(struct asset_nx2_usb_rank *rank);

char *asset_nx2_usb_rank_to_string(const struct asset_nx2_usb_rank *rank);

void asset_nx2_usb_rank_decrypt(uint8_t *buf, size_t len);

void asset_nx2_usb_rank_encrypt(uint8_t *buf, size_t len);

#endif