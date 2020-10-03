#ifndef ASSET_F2_USB_RANK_H
#define ASSET_F2_USB_RANK_H

#include <stdint.h>

struct asset_f2_usb_rank {
    // TODO
} __attribute__((__packed__));

struct asset_f2_usb_rank* asset_f2_usb_rank_new(void);

// update checksum and prepare profile to get encrypted
void asset_f2_usb_rank_finalize(struct asset_f2_usb_rank* rank);

char* asset_f2_usb_rank_to_string(const struct asset_f2_usb_rank* rank);

void asset_f2_usb_rank_decrypt(uint8_t* buf, size_t len);

void asset_f2_usb_rank_encrypt(uint8_t* buf, size_t len);

#endif