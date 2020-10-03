/**
 * Hasp emulator (Fiesta 2 and up)
 */
#ifndef SEC_HASP_H
#define SEC_HASP_H

#include <stdint.h>
#include <stdlib.h>

/**
 * A single entry in the key table
 */
struct sec_hasp_key {
    uint8_t req[64];
    uint8_t resp[64];
} __attribute__((packed));

/**
 * Initialize the hasp emulation module. The dongle for this
 * was used on Prime and up
 *
 * @param key_data Pointer to loaded key data (key table)
 * @param len Length of the buffer
 */
void sec_hasp_init(const uint8_t* key_data, size_t len);

int sec_hasp_api_login(int feature, int vendor_code, int* handle);

int sec_hasp_api_logout(int handle);

unsigned int sec_hasp_api_getid(void);

int sec_hasp_api_decrypt(int handle, void* buffer, size_t length);

#endif