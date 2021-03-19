#ifndef SEC_HASP_HANDLER_H
#define SEC_HASP_HANDLER_H

#include <stdint.h>
#include <stdlib.h>

struct sec_hasp_handler_header {
  uint32_t packet_size;
  uint16_t head_value;
  uint32_t transaction_num;
  uint32_t session_id;
  uint16_t unknown;
  uint16_t packet_type;
  uint32_t tail_value;
} __attribute((__packed__));

struct sec_hasp_handler_req {
  struct sec_hasp_handler_header header;
  uint8_t payload[];
};

struct sec_hasp_handler_resp {
  struct sec_hasp_handler_header header;
  uint8_t payload[];
};

struct sec_hasp_handler_transaction {
  size_t req_resp_max_size;
  struct sec_hasp_handler_req *req;
  struct sec_hasp_handler_resp *resp;
};

void sec_hasp_handler_init(const uint8_t *key_data, size_t len);

struct sec_hasp_handler_transaction *
sec_hasp_handler_alloc_transaction(size_t max_payload_size);

void sec_hasp_handler_free_transaction(
    struct sec_hasp_handler_transaction *transaction);

void sec_hasp_handler_do_transaction(
    struct sec_hasp_handler_transaction *transaction);

#endif
