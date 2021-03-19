#define LOG_MODULE "sec-microdog34"

#include <stdlib.h>
#include <string.h>

#include "util/log.h"
#include "util/mem.h"
#include "util/str.h"

#define IOCTL_REQUEST_MD_XACT 0x6B00
#define GOLD_SALT 0x646C6F47
#define MAGIC 0x484D

enum sec_microdog34_req_opcode {
  SEC_MICRODOG34_OP_DOG_CHECK = 0x01,
  SEC_MICRODOG34_OP_READ_DOG = 0x02,
  SEC_MICRODOG34_OP_WRITE_DOG = 0x03,
  SEC_MICRODOG34_OP_CONVERT = 0x04,
  SEC_MICRODOG34_OP_SET_PASS = 0x07,
  SEC_MICRODOG34_OP_SET_SHARE = 0x08,
  SEC_MICRODOG34_OP_GET_LOCK_NO = 0x0B,
  SEC_MICRODOG34_OP_LOGIN = 0x14,
  SEC_MICRODOG34_OP_DOG_CASCADE = 0x15,
  SEC_MICRODOG34_OP_RAINBOW_TABLE_ADD = 0x539,
  SEC_MICRODOG34_OP_DOG_SERIAL = 0x53A,
  SEC_MICRODOG34_OP_SET_MFG_SERIAL = 0x53B,
  SEC_MICRODOG34_OP_SET_VID = 0x53C,
  SEC_MICRODOG34_OP_RELOAD_RAINBOW_TABLE = 0x53D
};

struct sec_microdog34_req {
  uint16_t magic;
  uint16_t opcode;
  uint32_t dog_serial;
  uint32_t mask_key;
  uint16_t dog_addr;
  uint16_t dog_bytes;
  uint8_t dog_data[256];
  uint32_t dog_password;
  uint8_t host_id;
} __attribute__((__packed__));

enum sec_microdog34_resp_code {
  SEC_MICRODOG34_RESP_NO_ERR = 0,
  SEC_MICRODOG34_RESP_ERR_PASS = 0x2745
};

struct sec_microdog34_resp {
  uint32_t dog_serial;
  uint32_t return_code;
  uint8_t dog_data[200];
} __attribute__((__packed__));

struct sec_microdog34_header {
  uint32_t serial;
  uint32_t password;
  uint8_t vendor_id[8];
  uint32_t mfg_serial;
  uint8_t flash_memory[200];
  uint32_t num_keys;
};

struct sec_microdog34_key {
  uint32_t response;
  uint32_t algorithm;
  uint32_t req_len;
  uint8_t request[64];
};

static void sec_microdog34_dispatch(
    struct sec_microdog34_req *req, struct sec_microdog34_resp *resp);
static void sec_microdog34_crypt_req(struct sec_microdog34_req *req);
static void sec_microdog34_crypt_resp(
    struct sec_microdog34_req *req, struct sec_microdog34_resp *resp);
static void sec_microdog34_dump_req(struct sec_microdog34_req *req);
static void sec_microdog34_dump_resq(struct sec_microdog34_resp *resp);
static uint32_t
sec_microdog34_convert_req(const uint8_t *req_data, uint16_t req_len);

static struct sec_microdog34_header sec_microdog34_header;
static struct sec_microdog34_key *sec_microdog34_key_table;

static uint8_t sec_microdog34_share;

void sec_microdog34_init(const uint8_t *key_data, size_t len)
{
  memcpy(
      &sec_microdog34_header, key_data, sizeof(struct sec_microdog34_header));

  if (len - sizeof(struct sec_microdog34_header) <
      sec_microdog34_header.num_keys * sizeof(struct sec_microdog34_key)) {
    log_error(
        "Loading key table failed, incomplete key table "
        "(num_keys entry: %d, num_keys size: %d",
        sec_microdog34_header.num_keys,
        (len - sizeof(struct sec_microdog34_header)) /
            sizeof(struct sec_microdog34_key));
    return;
  }

  sec_microdog34_key_table = util_xmalloc(
      sec_microdog34_header.num_keys * sizeof(struct sec_microdog34_key));

  memcpy(
      sec_microdog34_key_table,
      key_data + sizeof(struct sec_microdog34_header),
      sec_microdog34_header.num_keys * sizeof(struct sec_microdog34_key));

  log_info(
      "Loaded %d keys, serial 0x%X, password 0x%X",
      sec_microdog34_header.num_keys,
      sec_microdog34_header.serial,
      sec_microdog34_header.password);

  sec_microdog34_share = 0;
}

int sec_microdog34_process(int request, void *data)
{
  if (request == IOCTL_REQUEST_MD_XACT) {
    struct sec_microdog34_req req;
    struct sec_microdog34_resp resp;

    memset(&resp, 0, sizeof(struct sec_microdog34_resp));

    // where to get the data? weird stuff, but that's how it's done
    // it really has to be like that, otherwise we get trash from data
    uint32_t req_data_offset = *(unsigned int *) data;
    memcpy(&req, (void *) req_data_offset, sizeof(struct sec_microdog34_req));

    sec_microdog34_crypt_req(&req);
    sec_microdog34_dispatch(&req, &resp);
    sec_microdog34_crypt_resp(&req, &resp);

    // again, same as req data
    // it really has to be like that, otherwise we get trash from data
    uint32_t resp_data_offset = *(unsigned int *) data - 272;
    memcpy(
        (void *) resp_data_offset, &resp, sizeof(struct sec_microdog34_resp));

    return 0;
  }

  return -1;
}

static void sec_microdog34_dispatch(
    struct sec_microdog34_req *req, struct sec_microdog34_resp *resp)
{
  struct sec_microdog34_key *key;

  resp->dog_serial = req->dog_serial;

  if (req->magic != MAGIC) {
    log_error("Bad magic 0x%X for request", req->magic);
  }

  sec_microdog34_dump_req(req);

  switch (req->opcode) {
    case SEC_MICRODOG34_OP_DOG_CHECK:
      log_debug("SEC_MICRODOG34_OP_DOG_CHECK");
      break;

    case SEC_MICRODOG34_OP_READ_DOG:
      log_debug("SEC_MICRODOG34_OP_READ_DOG");

      if (req->dog_password == sec_microdog34_header.password) {
        memcpy(
            resp->dog_data,
            sec_microdog34_header.flash_memory + req->dog_addr,
            req->dog_bytes);
      } else {
        resp->return_code = SEC_MICRODOG34_RESP_ERR_PASS;
      }

      break;

    case SEC_MICRODOG34_OP_WRITE_DOG:
      log_debug("SEC_MICRODOG34_OP_WRITE_DOG");

      if (req->dog_password == sec_microdog34_header.password) {
        memcpy(
            sec_microdog34_header.flash_memory + req->dog_addr,
            req->dog_data,
            req->dog_bytes);
      } else {
        resp->return_code = SEC_MICRODOG34_RESP_ERR_PASS;
      }

      break;

    case SEC_MICRODOG34_OP_CONVERT:
      log_debug("SEC_MICRODOG34_OP_CONVERT");

      uint32_t resp_data =
          sec_microdog34_convert_req(req->dog_data, req->dog_bytes);
      memcpy(resp->dog_data, &resp_data, sizeof(uint32_t));

      break;

    case SEC_MICRODOG34_OP_SET_PASS:
      log_debug(
          "SEC_MICRODOG34_OP_SET_PASS: 0x%08X", *((uint32_t *) req->dog_data));

      if (req->dog_password == sec_microdog34_header.password) {
        sec_microdog34_header.password = *((uint32_t *) req->dog_data);
      } else
        resp->return_code = SEC_MICRODOG34_RESP_ERR_PASS;

      break;

    case SEC_MICRODOG34_OP_SET_SHARE:
      log_debug("SEC_MICRODOG34_OP_SET_SHARE");

      sec_microdog34_share = req->dog_data[0];
      resp->dog_data[0] = sec_microdog34_share;

      break;

    case SEC_MICRODOG34_OP_GET_LOCK_NO:
      log_debug(
          "SEC_MICRODOG34_OP_GET_LOCK_NO: 0x%08X",
          sec_microdog34_header.mfg_serial);

      memcpy(resp->dog_data, &sec_microdog34_header.mfg_serial, 4);

      break;

    case SEC_MICRODOG34_OP_LOGIN:
      log_debug("SEC_MICRODOG34_OP_LOGIN");

      memcpy(
          resp->dog_data,
          sec_microdog34_header.vendor_id,
          sizeof(sec_microdog34_header.vendor_id));

      break;

    case SEC_MICRODOG34_OP_DOG_CASCADE:
      log_debug("SEC_MICRODOG34_OP_DOG_CASCADE");

      resp->dog_data[0] = req->dog_data[0];

      break;

    case SEC_MICRODOG34_OP_RAINBOW_TABLE_ADD:
      log_debug("SEC_MICRODOG34_OP_RAINBOW_TABLE_ADD");

      /* expand table */
      sec_microdog34_key_table = util_xrealloc(
          sec_microdog34_key_table,
          (sec_microdog34_header.num_keys + 1) *
              sizeof(struct sec_microdog34_key));

      key = &sec_microdog34_key_table[sec_microdog34_header.num_keys];
      key->response = req->dog_password;
      /* algorithm(last 4 bytes of flash memory) */
      key->algorithm =
          *((uint32_t *) (sec_microdog34_header.flash_memory + 196));
      key->req_len = req->dog_bytes;
      memcpy(key->request, req->dog_data, sizeof(key->request));

      sec_microdog34_header.num_keys++;

      break;

    case SEC_MICRODOG34_OP_DOG_SERIAL:
      log_debug(
          "SEC_MICRODOG34_OP_DOG_SERIAL: 0x%08X",
          *((uint32_t *) req->dog_data));

      sec_microdog34_header.serial = *((uint32_t *) req->dog_data);

      break;

    case SEC_MICRODOG34_OP_SET_MFG_SERIAL:
      log_debug(
          "SEC_MICRODOG34_OP_SET_MFG_SERIAL: 0x%08X ",
          *((uint32_t *) req->dog_data));

      sec_microdog34_header.mfg_serial = *((uint32_t *) req->dog_data);

      break;

    case SEC_MICRODOG34_OP_SET_VID:
      log_debug("SEC_MICRODOG34_OP_SET_VID");

      memcpy(
          sec_microdog34_header.vendor_id,
          req->dog_data,
          sizeof(sec_microdog34_header.vendor_id));

      break;

    case SEC_MICRODOG34_OP_RELOAD_RAINBOW_TABLE:
      log_debug("SEC_MICRODOG34_OP_RELOAD_RAINBOW_TABLE");

      /* nothing to load here */
      break;

    default:
      log_warn("Unsupported opcode: %04X", req->opcode);
      break;
  }

  sec_microdog34_dump_resq(resp);
}

static void sec_microdog34_crypt_req(struct sec_microdog34_req *req)
{
  uint32_t tmp_mask = (req->mask_key + GOLD_SALT) & 0xFFFFFFFF;
  uint8_t tmb_mask[4] = {0, 0, 0, 0};
  memcpy(tmb_mask, &tmp_mask, 4);

  req->dog_addr ^= (tmp_mask & 0xFFFF);
  req->dog_bytes ^= (tmp_mask & 0xFFFF);
  req->dog_password ^= tmp_mask;
  req->host_id ^= (tmp_mask & 0xFF);

  for (int i = 0; i < sizeof(req->dog_data); i++) {
    req->dog_data[i] ^= tmb_mask[i % 4];
  }
}

static void sec_microdog34_crypt_resp(
    struct sec_microdog34_req *req, struct sec_microdog34_resp *resp)
{
  uint32_t tmp_mask = (req->mask_key + GOLD_SALT) & 0xFFFFFFFF;
  uint8_t tmb_mask[4] = {0, 0, 0, 0};
  memcpy(tmb_mask, &tmp_mask, 4);

  for (int i = 0; i < sizeof(resp->dog_data); i++) {
    resp->dog_data[i] ^= tmb_mask[i % 4];
  }
}

static void sec_microdog34_dump_req(struct sec_microdog34_req *req)
{
  char *data_tmp = util_str_buffer(req->dog_data, sizeof(req->dog_data));

  log_debug(
      "Dog request:\n"
      "magic: 0x%X\n"
      "opcode: 0x%X\n"
      "dog_serial: 0x%X\n"
      "mask_key: 0x%X\n"
      "dog_addr: 0x%X\n"
      "dog_bytes: 0x%X\n"
      "dog_data: %s\n"
      "dog_pass: 0x%X\n"
      "host_id: 0x%X",
      req->magic,
      req->opcode,
      req->dog_serial,
      req->mask_key,
      req->dog_addr,
      req->dog_bytes,
      data_tmp,
      req->dog_password,
      req->host_id);

  free(data_tmp);
}

static void sec_microdog34_dump_resq(struct sec_microdog34_resp *resp)
{
  char *data_tmp = util_str_buffer(resp->dog_data, sizeof(resp->dog_data));

  log_debug(
      "Dog response:\n"
      "dog_serial: 0x%X\n"
      "return_code: 0x%X\n"
      "dog_data: %s\n",
      resp->dog_serial,
      resp->return_code,
      data_tmp);

  free(data_tmp);
}

static uint32_t
sec_microdog34_convert_req(const uint8_t *req_data, uint16_t req_len)
{
  uint32_t cur_algo;

  cur_algo = 0;
  memcpy(&cur_algo, sec_microdog34_header.flash_memory + 196, 4);

  for (uint32_t i = 0; i < sec_microdog34_header.num_keys; i++) {
    struct sec_microdog34_key *key = &sec_microdog34_key_table[i];

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