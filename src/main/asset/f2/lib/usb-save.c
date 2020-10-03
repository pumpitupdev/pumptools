#include "util/adler32.h"

#include "usb-save.h"

#define ENC_DEC_INIT_SEED 0xEBADA1

struct asset_f2_usb_save* asset_f2_usb_save_new(void)
{
    // TODO
    return malloc(sizeof(struct asset_f2_usb_save));
}

void asset_f2_usb_save_finalize(struct asset_f2_usb_save* save)
{
    /* don't include the checksum field */
    save->header.adler32 = util_adler32_calc(save->header.adler_seed,
        &save->header.adler_seed,
        sizeof(struct asset_f2_usb_save) - sizeof(uint32_t));
}

char* asset_f2_usb_save_to_string(const struct asset_f2_usb_save* save)
{
    // TODO
    return "";
}

void asset_f2_usb_save_decrypt(uint8_t* buf, size_t len)
{
    uint32_t seed = ENC_DEC_INIT_SEED;

	for (int i = 0; i < len; i++){
		uint8_t smbuff = buf[i];
		buf[i] ^= (seed >> 8) & 0xff ;
		seed = 0x68993 * (smbuff + seed) + 0x4FDCF;
	}
}

void asset_f2_usb_save_encrypt(uint8_t* buf, size_t len)
{
    uint32_t seed = ENC_DEC_INIT_SEED;

	for (size_t i = 0; i < len; i++){
		uint8_t smbuff = buf[i] ^ (seed >> 8) & 0xff;
		buf[i] = smbuff ;
		seed = 0x68993 * (smbuff + seed) + 0x4FDCF;
	}
}