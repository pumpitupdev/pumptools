/**
 * Microdog 3.4 emulator
 *
 * Credits to batteryshark for the emulation code and key files
 */
#ifndef SEC_MICRODOG34_H
#define SEC_MICRODOG34_H

/**
 * Header of a key file containing attributes of the dongle. This is followed
 * by an arbitrary number of keys
 */
struct sec_microdog34_header {
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
struct sec_microdog34_key {
  uint32_t response;
  uint32_t algorithm;
  uint32_t req_len;
  uint8_t request[64];
};

/**
 * Initialize the microdog (3.4) emulation module. The dongle for this
 * was used on Exceed2, Zero and NX
 *
 * @param key_data Pointer to loaded key data (dog attributes, key table)
 * @param len Length of the buffer
 */
void sec_microdog34_init(const uint8_t *key_data, size_t len);

/**
 * Process an incoming request to the dongle
 *
 * @param request IOCTL request for the dongle
 * @param data Pointer to a buffer with request data. Response is returned
 *        to the same buffer (request overwritten)
 * @return 0 and success, -1 on failure
 */
int sec_microdog34_process(int request, void *data);

#endif