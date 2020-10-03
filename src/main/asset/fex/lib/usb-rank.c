#define LOG_MODULE "fex-profile-rank"

#include "usb-rank.h"

struct asset_fex_usb_rank* asset_fex_usb_rank_new(void)
{
    return malloc(sizeof(struct asset_fex_usb_rank));
}

void asset_fex_usb_rank_finalize(struct asset_fex_usb_rank* rank)
{

}

char* asset_fex_usb_rank_to_string(const struct asset_fex_usb_rank* rank)
{
    return malloc(0);
}

void asset_fex_usb_rank_decrypt(uint8_t* buf, size_t len)
{
	for (size_t a = len - 1; a > 0; --a) {
		buf[a] = (buf[a] ^ buf[a - 1]) + ((a * 1234567) >> 8);
	}
}

void asset_fex_usb_rank_encrypt(uint8_t* buf, size_t len)
{
	for (size_t a = 1; a < len; ++a) {
		buf[a] = (buf[a] - ((a * 1234567) >> 8)) ^ buf[a - 1];
	}
}