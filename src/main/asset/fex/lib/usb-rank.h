#ifndef ASSET_FEX_USB_RANK_H
#define ASSET_FEX_USB_RANK_H

#include <stdint.h>
#include <stdlib.h>

struct asset_fex_usb_rank_header {
  uint32_t adler32;
  /* NULL terminated */
  char player_id[8];
} __attribute__((__packed__));

struct asset_fex_usb_rank {
  struct asset_fex_usb_rank_header header;
} __attribute__((__packed__));

struct asset_fex_usb_rank *asset_fex_usb_rank_new(void);

// update checksum and prepare profile to get encrypted
void asset_fex_usb_rank_finalize(struct asset_fex_usb_rank *rank);

char *asset_fex_usb_rank_to_string(const struct asset_fex_usb_rank *rank);

void asset_fex_usb_rank_decrypt(uint8_t *buf, size_t len);

void asset_fex_usb_rank_encrypt(uint8_t *buf, size_t len);

#endif