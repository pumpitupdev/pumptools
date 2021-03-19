#ifndef SEC_HASP_OBJECT_LOGIN_REQ_H
#define SEC_HASP_OBJECT_LOGIN_REQ_H

#include <stdint.h>
#include <stdlib.h>

struct sec_hasp_object_login_req {
  uint16_t oid;
  uint32_t pid;
  uint32_t tid;
  uint32_t hasp_uid;
  uint32_t vendor_id;
  uint32_t feature_id;
  char *username;
  char *machine_name;
  char *login_type;
  uint32_t api_version_major;
  uint32_t api_version_minor;
  uint32_t timestamp;
  uint32_t val_8b;
  uint32_t val_8c;
  uint32_t val_8d;
  uint32_t val_8e;
  uint32_t volume_serial;
  uint32_t val_90;
  uint32_t hasp_handle;
};

struct sec_hasp_object_login_req *sec_hasp_object_login_req_init();

void sec_hasp_object_login_req_free(struct sec_hasp_object_login_req *obj);

size_t sec_hasp_object_login_req_encode(
    struct sec_hasp_object_login_req *obj, uint8_t *buffer, size_t len);

void sec_hasp_object_login_req_decode(
    uint8_t *buffer, size_t len, struct sec_hasp_object_login_req *obj);

#endif
