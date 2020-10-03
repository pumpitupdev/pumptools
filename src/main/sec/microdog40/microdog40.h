/**
 * Microdog 4.0 emulator
 *
 * Credits to batteryshark for the emulation code and key files
 */
#ifndef SEC_MICRODOG40_H
#define SEC_MICRODOG40_H

#include <stdbool.h>
#include <stdint.h>

/**
 * Header of a key file containing attributes of the dongle. This is followed
 * by an arbitrary number of keys
 */
struct sec_microdog40_header {
    uint32_t serial;
    uint32_t password;
    uint8_t vendor_id[8];
    uint32_t mfg_serial;
    uint8_t flash_memory[200];
    uint32_t num_keys;
};

/**
 * A single entry in the key table (comes after the header)
 */
struct sec_microdog40_key {
    uint32_t response;
    uint32_t algorithm;
    uint32_t req_len;
    uint8_t request[64];
};

/**
 * Timestamp for a dongle request
 */
struct sec_microdog40_timestamp {
    uint16_t year;
    uint16_t month;
    uint16_t day;
    uint16_t hour;
    uint16_t minute;
    uint16_t second;
    uint8_t padding[3];
} __attribute((__packed__));

/**
 * Request to the dongle as part of a transaction
 */
struct sec_microdog40_req {
    uint16_t magic;
    uint8_t req_type;
    uint32_t mask_key1;
    uint8_t dog_cascade;
    uint32_t mfg_serial;
    uint32_t mask_key2;
    uint16_t dog_addr;
    uint16_t payload_size;
    uint32_t mask_key3;
    uint8_t payload[256];
    uint32_t dog_password;
    uint8_t host_id;
    uint32_t mask_key4;
    struct sec_microdog40_timestamp timestamp;
} __attribute((__packed__));

/**
 * Dongle response as part of a transaction
 */
struct sec_microdog40_resp {
    uint16_t magic;
    uint32_t mask_key1;
    uint8_t req_type;
    uint32_t mask_key2;
    uint32_t krnl_retval;
    uint32_t mask_key3;
    uint8_t payload[256];
    uint32_t mask_key4;
    uint8_t padding[9];
} __attribute((__packed__));

/**
 * Dongle emulator transaction data
 */
struct sec_microdog40_transaction {
    uint32_t header;
    struct sec_microdog40_req request;
    struct sec_microdog40_resp response;
} __attribute((__packed__));

/**
 * Initialize the microdog (4.0) emulation module. The dongle for this
 * was used on NX2, NXA, Fiesta and FiestaEX
 *
 * @param key_data Pointer to loaded key data (dog attributes, key table)
 * @param len Length of the buffer
 */
void sec_microdog40_init(const uint8_t* key_data, size_t len);

/**
 * Execute a transaction on the dongle
 *
 * @param transaction Transaction containing the request. The response is
 *        written to it after successfully processing the request
 * @return True if the request was successfully processed and a response was
 *         written, false on failure
 */
bool sec_microdog40_do_transaction(struct sec_microdog40_transaction* transaction);

#endif