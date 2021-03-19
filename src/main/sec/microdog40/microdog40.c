#define LOG_MODULE "sec-microdog40"

#include <string.h>

#include "crypt/aes.h"
#include "crypt/md5.h"

#include "sec/microdog40/microdog40.h"

#include "util/log.h"
#include "util/mem.h"
#include "util/str.h"

#define REQHEAD_SIZE 0x120
#define REQTAIL_SIZE 0x10
#define PROTOCOL_VERSION 0x11

enum sec_microdog40_req_opcode {
  SEC_MICRODOG40_REQ_OPCODE_DOG_CHECK = 0x01,
  SEC_MICRODOG40_REQ_OPCODE_SET_SHARE = 0x08,
  SEC_MICRODOG40_REQ_OPCODE_GET_LOCK_NO = 0x0B,
  SEC_MICRODOG40_REQ_OPCODE_GET_ID = 0x14,
  SEC_MICRODOG40_REQ_OPCODE_CONVERT = 0x40,
  SEC_MICRODOG40_REQ_OPCODE_AUTH_STEP_1 = 0x65,
  SEC_MICRODOG40_REQ_OPCODE_AUTH_STEP_2 = 0x67,
  SEC_MICRODOG40_REQ_OPCODE_AUTH_STEP_3 = 0x66
};

struct sec_microdog40_month_key_hash_input {
  uint8_t ssk_prefix[25];
  uint16_t year;
  uint8_t const45;
  uint16_t month;
} __attribute((__packed__));

static const uint8_t sec_microdog40_shared_secret[30] = {
    0x2A, 0x2F, 0xED, 0x5E, 0x49, 0x26, 0x40, 0x19, 0x40, 0x40,
    0xE2, 0x51, 0xAA, 0xFA, 0xDB, 0xCB, 0x67, 0x21, 0x4C, 0xA4,
    0x10, 0x7E, 0x51, 0x22, 0x25, 0x11, 0x2B, 0x3C, 0x46, 0x5E};

static const uint16_t sec_microdog40_magic = 0x484D;

static struct sec_microdog40_header sec_microdog40_header;
static struct sec_microdog40_key *sec_microdog40_key_table;

static uint8_t sec_microdog40_share;

static void sec_microdog40_log_req(const struct sec_microdog40_req *req)
{
  char *data_tmp = util_str_buffer(req->payload, sizeof(req->payload));

  log_debug(
      "Processing request:\n"
      "magic: 0x%X\n"
      "req_type: 0x%X\n"
      "mask_key1: 0x%X\n"
      "dog_cascade: 0x%X\n"
      "mfg_serial: 0x%X\n"
      "mask_key2: 0x%X\n"
      "dog_addr: 0x%X\n"
      "payload_size: %d\n"
      "mask_key3: 0x%X\n"
      "payload: %s\n"
      "dog_password: 0x%X\n"
      "host_id: 0x%X\n"
      "mask_key4: 0x%X\n"
      "timestamp.year: %d\n"
      "timestamp.month: %d\n"
      "timestamp.day: %d\n"
      "timestamp.hour: %d\n"
      "timestamp.minute: %d\n"
      "timestamp.second: %d",
      req->magic,
      req->req_type,
      req->mask_key1,
      req->dog_cascade,
      req->mfg_serial,
      req->mask_key2,
      req->dog_addr,
      req->payload_size,
      req->mask_key3,
      data_tmp,
      req->dog_password,
      req->host_id,
      req->mask_key4,
      req->timestamp.year,
      req->timestamp.month,
      req->timestamp.day,
      req->timestamp.hour,
      req->timestamp.minute,
      req->timestamp.second);

  free(data_tmp);
}

static void sec_microdog40_get_month_key(
    uint8_t *month_key, const struct sec_microdog40_timestamp *timestamp)
{
  struct sec_microdog40_month_key_hash_input hash_in;

  memcpy(
      hash_in.ssk_prefix,
      sec_microdog40_shared_secret,
      sizeof(hash_in.ssk_prefix));
  hash_in.year = timestamp->year;
  hash_in.month = timestamp->month;
  hash_in.const45 = 45;

  crypt_md5_hash(
      month_key,
      (const uint8_t *) &hash_in,
      sizeof(struct sec_microdog40_month_key_hash_input));
}

static void sec_microdog40_dec_req(
    const struct sec_microdog40_req *enc_req,
    struct sec_microdog40_req *dec_req)
{
  uint8_t key[16];

  crypt_md5_hash(
      key, sec_microdog40_shared_secret, sizeof(sec_microdog40_shared_secret));
  crypt_aes_dec(
      key,
      CRYPT_AES_KEY_LENGTH_16_BYTES,
      ((uint8_t *) dec_req) + REQHEAD_SIZE,
      ((uint8_t *) enc_req) + REQHEAD_SIZE,
      REQTAIL_SIZE);

  sec_microdog40_get_month_key(key, &dec_req->timestamp);
  crypt_aes_dec(
      key,
      CRYPT_AES_KEY_LENGTH_16_BYTES,
      ((uint8_t *) dec_req),
      ((uint8_t *) enc_req),
      REQHEAD_SIZE);

  dec_req->magic ^= dec_req->mask_key4;
  dec_req->req_type ^= dec_req->mask_key4;
  dec_req->mask_key1 ^= dec_req->mask_key4;
  dec_req->dog_cascade ^= dec_req->mask_key4;
  dec_req->mfg_serial ^= dec_req->mask_key4;
  dec_req->mask_key2 ^= dec_req->mask_key4;
  dec_req->dog_addr ^= dec_req->mask_key4;
  dec_req->payload_size ^= dec_req->mask_key4;
  dec_req->mask_key3 ^= dec_req->mask_key4;
  dec_req->dog_password ^= dec_req->mask_key4;
  dec_req->host_id ^= dec_req->mask_key4;

  for (int i = 0; i < 64; i++) {
    ((uint32_t *) dec_req->payload)[i] ^= dec_req->mask_key4;
  }

  dec_req->magic ^= dec_req->mask_key3;
  dec_req->req_type ^= dec_req->mask_key3;
  dec_req->mask_key1 ^= dec_req->mask_key3;
  dec_req->dog_cascade ^= dec_req->mask_key3;
  dec_req->mfg_serial ^= dec_req->mask_key3;
  dec_req->mask_key2 ^= dec_req->mask_key3;
  dec_req->dog_addr ^= dec_req->mask_key3;
  dec_req->payload_size ^= dec_req->mask_key3;

  dec_req->magic ^= dec_req->mask_key2;
  dec_req->req_type ^= dec_req->mask_key2;
  dec_req->mask_key1 ^= dec_req->mask_key2;
  dec_req->dog_cascade ^= dec_req->mask_key2;
  dec_req->mfg_serial ^= dec_req->mask_key2;

  dec_req->magic ^= dec_req->mask_key1;
  dec_req->req_type ^= dec_req->mask_key1;
}

/* unused */
/*
static void sec_microdog40_dec_resp(const struct sec_microdog40_resp* enc_resp,
        struct sec_microdog40_resp* dec_resp,
        const struct sec_microdog40_timestamp* timestamp)
{
    uint8_t key[16];

    sec_microdog40_get_month_key(key, timestamp);
    crypt_aes_dec(key, CRYPT_AES_KEY_LENGTH_16_BYTES, (uint8_t*) dec_resp,
        (uint8_t*) enc_resp, sizeof(struct sec_microdog40_resp));

    for (int i = 0 ; i < 64 ; i++) {
        ((uint32_t *) dec_resp->payload)[i] ^= dec_resp->mask_key4;
    }

    dec_resp->mask_key3 ^= dec_resp->mask_key4;
    dec_resp->krnl_retval ^= dec_resp->mask_key4;
    dec_resp->mask_key2 ^= dec_resp->mask_key4;
    dec_resp->req_type ^= dec_resp->mask_key4;
    dec_resp->mask_key1 ^= dec_resp->mask_key4;
    dec_resp->magic ^= dec_resp->mask_key4;

    dec_resp->krnl_retval ^= dec_resp->mask_key3;
    dec_resp->mask_key2 ^= dec_resp->mask_key3;
    dec_resp->req_type ^= dec_resp->mask_key3;
    dec_resp->mask_key1 ^= dec_resp->mask_key3;
    dec_resp->magic ^= dec_resp->mask_key3;

    dec_resp->req_type ^= dec_resp->mask_key2;
    dec_resp->mask_key1 ^= dec_resp->mask_key2;
    dec_resp->magic ^= dec_resp->mask_key2;

    dec_resp->magic ^= dec_resp->mask_key1;
}
*/

/* unused */
/*
static void sec_microdog40_enc_req(const struct sec_microdog40_req* dec_req,
        struct sec_microdog40_req* enc_req)
{
     uint8_t key[16];

    // Mask keying not necessary

    sec_microdog40_get_month_key(key, &dec_req->timestamp);
    crypt_aes_enc(key, CRYPT_AES_KEY_LENGTH_16_BYTES, (uint8_t*) enc_req,
        (uint8_t*) dec_req, sizeof(struct sec_microdog40_req));

    crypt_md5_hash(key, sec_microdog40_shared_secret,
        sizeof(sec_microdog40_shared_secret));
    crypt_aes_enc(key, CRYPT_AES_KEY_LENGTH_16_BYTES, ((uint8_t*) enc_req) +
        REQHEAD_SIZE, ((uint8_t*) &dec_req) + REQHEAD_SIZE, REQTAIL_SIZE);
}
*/

static void sec_microdog40_encrypt_resp(
    const struct sec_microdog40_resp *dec_resp,
    struct sec_microdog40_resp *enc_resp,
    const struct sec_microdog40_timestamp *timestamp)
{
  uint8_t key[16];

  sec_microdog40_get_month_key(key, timestamp);
  crypt_aes_enc(
      key,
      CRYPT_AES_KEY_LENGTH_16_BYTES,
      (uint8_t *) enc_resp,
      (uint8_t *) dec_resp,
      sizeof(struct sec_microdog40_resp));
}

static uint32_t
sec_microdog40_convert_req(const uint8_t *req_data, uint16_t req_len)
{
  uint32_t cur_algo;

  cur_algo = 0;
  memcpy(&cur_algo, sec_microdog40_header.flash_memory + 196, 4);

  for (uint32_t i = 0; i < sec_microdog40_header.num_keys; i++) {
    struct sec_microdog40_key *key = &sec_microdog40_key_table[i];

    if (key->algorithm == cur_algo) {

      if (key->req_len == req_len) {

        if (!memcmp(key->request, req_data, req_len)) {
          char *tmp = util_str_buffer(req_data, req_len);
          log_debug("Converting: %s ==> %X", tmp, key->response);
          free(tmp);
          return key->response;
        }
      }
    }
  }

  char *tmp = util_str_buffer(req_data, req_len);
  log_warn("Converting request %s failed, no matching response found", tmp);
  free(tmp);
  return 0;
}

static void sec_microdog40_dispatch(
    const struct sec_microdog40_req *req, struct sec_microdog40_resp *resp)
{
  // prepare response
  resp->magic = sec_microdog40_magic;
  resp->req_type = req->req_type;

  if (req->magic != sec_microdog40_magic) {
    log_warn("Bad magic for request: %X", req->magic);
  }

  sec_microdog40_log_req(req);

  switch (req->req_type) {
    case SEC_MICRODOG40_REQ_OPCODE_DOG_CHECK: {
      log_debug("SEC_MICRODOG40_REQ_OPCODE_DOG_CHECK");
      /* no-op */
      break;
    }

    case SEC_MICRODOG40_REQ_OPCODE_SET_SHARE: {
      log_debug("SEC_MICRODOG40_REQ_OPCODE_SET_SHARE");

      sec_microdog40_share = req->payload[0];
      break;
    }

    case SEC_MICRODOG40_REQ_OPCODE_GET_LOCK_NO: {
      log_debug(
          "SEC_MICRODOG40_REQ_OPCODE_GET_LOCK_NO 0x%08X",
          sec_microdog40_header.mfg_serial);

      memcpy(resp->payload, &sec_microdog40_header.mfg_serial, 4);
      break;
    }

    case SEC_MICRODOG40_REQ_OPCODE_GET_ID: {
      log_debug(
          "SEC_MICRODOG40_REQ_OPCODE_GET_ID 0x%02X 0x%02X 0x%02X "
          "0x%02X 0x%02X 0x%02X 0x%02X 0x%02X",
          sec_microdog40_header.vendor_id[0],
          sec_microdog40_header.vendor_id[1],
          sec_microdog40_header.vendor_id[2],
          sec_microdog40_header.vendor_id[3],
          sec_microdog40_header.vendor_id[4],
          sec_microdog40_header.vendor_id[5],
          sec_microdog40_header.vendor_id[6],
          sec_microdog40_header.vendor_id[7]);

      memcpy(
          resp->payload,
          sec_microdog40_header.vendor_id,
          sizeof(sec_microdog40_header.vendor_id));

      break;
    }

    case SEC_MICRODOG40_REQ_OPCODE_CONVERT: {
      uint32_t resp_data;

      log_debug("SEC_MICRODOG40_REQ_OPCODE_CONVERT");

      resp_data = sec_microdog40_convert_req(req->payload, req->payload_size);
      memcpy(resp->payload, &resp_data, sizeof(uint32_t));

      break;
    }

    case SEC_MICRODOG40_REQ_OPCODE_AUTH_STEP_1: {
      log_debug("SEC_MICRODOG40_REQ_OPCODE_AUTH_STEP_1");
      /* no-op */
      break;
    }

    case SEC_MICRODOG40_REQ_OPCODE_AUTH_STEP_2: {
      log_debug("SEC_MICRODOG40_REQ_OPCODE_AUTH_STEP_2");
      /* no-op */
      break;
    }

    case SEC_MICRODOG40_REQ_OPCODE_AUTH_STEP_3: {
      uint8_t month_key[16];

      log_debug("SEC_MICRODOG40_REQ_OPCODE_AUTH_STEP_3");

      sec_microdog40_get_month_key(month_key, &req->timestamp);

      crypt_aes_enc(
          month_key,
          CRYPT_AES_KEY_LENGTH_16_BYTES,
          resp->payload,
          req->payload,
          16);

      break;
    }

    default: {
      log_warn("Unsupported Opcode: %04X", req->req_type);
      resp->magic = 0; // mark to indicate error
      break;
    }
  }
}

void sec_microdog40_init(const uint8_t *key_data, size_t len)
{
  if (len < sizeof(struct sec_microdog40_header)) {
    log_error("Loading key table failed, incomplete header");
    return;
  }

  memcpy(
      &sec_microdog40_header, key_data, sizeof(struct sec_microdog40_header));

  if (len - sizeof(struct sec_microdog40_header) <
      sec_microdog40_header.num_keys * sizeof(struct sec_microdog40_key)) {
    log_error("Loading key table failed, incomplete key table");
    return;
  }

  sec_microdog40_key_table = util_xmalloc(
      sec_microdog40_header.num_keys * sizeof(struct sec_microdog40_key));

  memcpy(
      sec_microdog40_key_table,
      key_data + sizeof(struct sec_microdog40_header),
      sec_microdog40_header.num_keys * sizeof(struct sec_microdog40_key));

  log_info(
      "Loaded %d keys, serial 0x%X, password 0x%X",
      sec_microdog40_header.num_keys,
      sec_microdog40_header.serial,
      sec_microdog40_header.password);

  sec_microdog40_share = 0;
}

bool sec_microdog40_do_transaction(
    struct sec_microdog40_transaction *transaction)
{
  struct sec_microdog40_req dec_req;
  struct sec_microdog40_resp dec_resp;

  if (transaction->header != PROTOCOL_VERSION) {
    log_error("Wrong protocol version of transaction %X.", transaction->header);
    return false;
  }

  log_debug("Decrypting request...");
  sec_microdog40_dec_req(&transaction->request, &dec_req);

  sec_microdog40_dispatch(&dec_req, &dec_resp);

  /* no error */
  if (dec_resp.magic != 0) {
    transaction->header = 0;

    log_debug("Encrypting response...");
    sec_microdog40_encrypt_resp(
        &dec_resp, &transaction->response, &dec_req.timestamp);
  } else {
    transaction->header = 20023;
    log_error("Transaction failed.");
  }

  log_debug("Transaction successful");

  return true;
}
