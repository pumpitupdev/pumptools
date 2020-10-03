#ifndef ASSET_F2_UTIL_H
#define ASSET_F2_UTIL_H

#include <stdbool.h>

#include "usb-rank.h"
#include "usb-save.h"

// load from file and decrypt, returns decrypted struct
struct asset_f2_usb_rank* asset_f2_util_usb_rank_load_from_file(const char* path, bool encrypted);

// takes decrypted rank data, encrypts it and writes it to file
bool asset_f2_util_usb_rank_save_to_file(const char* path, const struct asset_f2_usb_rank* rank, bool encrypt);

// load from file and decrypt, returns decrypted struct
struct asset_f2_usb_save* asset_f2_util_usb_save_load_from_file(const char* path, bool encrypted);

// takes decrypted save data, encrypts it and writes it to file
bool asset_f2_util_usb_save_save_to_file(const char* path, const struct asset_f2_usb_save* save, bool encrypt);

#endif