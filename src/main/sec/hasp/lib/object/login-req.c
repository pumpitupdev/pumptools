#include <time.h>

#include "sec/hasp/const.h"
#include "sec/hasp/main.h"

#include "util/mem.h"

#include "login-req.h"

struct sec_hasp_object_login_req *sec_hasp_object_login_req_init()
{
  struct sec_hasp_object_login_req *obj =
      util_xmalloc(sizeof(struct sec_hasp_object_login_req));

  obj->oid = SEC_HASP_CONST_OID_LOGIN_REQ;
  obj->pid = 0;
  obj->tid = 0;
  obj->hasp_uid = 0;
  obj->vendor_id = 0;
  obj->feature_id = 0;
  obj->username = NULL;
  obj->machine_name = NULL;
  obj->login_type = NULL;
  obj->api_version_major = 0;
  obj->api_version_minor = 0;
  obj->timestamp = 0;
  obj->val_8b = 0;
  obj->val_8c = 0;
  obj->val_8d = 0;
  obj->val_8e = 0;
  obj->volume_serial = 0;
  obj->val_90 = 0;
  obj->hasp_handle = 0;

  return obj;
}

void sec_hasp_object_login_req_free(struct sec_hasp_object_login_req *obj)
{
  if (obj->username) {
    free(obj->username);
  }

  if (obj->machine_name) {
    free(obj->machine_name);
  }

  if (obj->login_type) {
    free(obj->login_type);
  }
}

size_t sec_hasp_object_login_req_encode(
    struct sec_hasp_object_login_req *obj, uint8_t *buffer, size_t len)
{
  struct sec_hasp_asn1_store *store =
      sec_hasp_asn1_object_store_alloc(obj->oid, 18);

  /* populate right before encoding */
  obj->timestamp = (uint32_t) time(NULL);

  sec_hasp_asn1_object_add_int(store, 0x80, obj->pid);
  sec_hasp_asn1_object_add_int(store, 0x81, obj->tid);
  sec_hasp_asn1_object_add_int(store, 0x82, obj->hasp_uid);
  sec_hasp_asn1_object_add_int(store, 0x83, obj->vendor_id);
  sec_hasp_asn1_object_add_int(store, 0x84, obj->feature_id);

  sec_hasp_asn1_object_add_str(store, 0x85, obj->username);
  sec_hasp_asn1_object_add_str(store, 0x86, obj->machine_name);
  sec_hasp_asn1_object_add_str(store, 0x87, obj->login_type);

  sec_hasp_asn1_object_add_int(store, 0x88, obj->api_version_major);
  sec_hasp_asn1_object_add_int(store, 0x89, obj->api_version_minor);

  sec_hasp_asn1_object_add_time(store, 0x8A, obj->timestamp);

  sec_hasp_asn1_object_add_int(store, 0x8B, obj->val_8b);
  sec_hasp_asn1_object_add_int(store, 0x8C, obj->val_8c);
  sec_hasp_asn1_object_add_int(store, 0x8D, obj->val_8d);
  sec_hasp_asn1_object_add_int(store, 0x8E, obj->val_8e);
  sec_hasp_asn1_object_add_int(store, 0x8F, obj->volume_serial);
  sec_hasp_asn1_object_add_int(store, 0x90, obj->val_90);
  sec_hasp_asn1_object_add_int(store, 0x91, obj->hasp_handle);

  return sec_hasp_asn1_encode(store, buffer, len);
}

void sec_hasp_object_login_req_decode(
    uint8_t *buffer, size_t len, struct sec_hasp_object_login_req *obj)
{
  struct sec_hasp_asn1_store *store =
      sec_hasp_asn1_object_store_alloc(obj->oid, 18);

  sec_hasp_asn1_object_set_int(store, 0x80);
  sec_hasp_asn1_object_set_int(store, 0x81);
  sec_hasp_asn1_object_set_int(store, 0x82);
  sec_hasp_asn1_object_set_int(store, 0x83);
  sec_hasp_asn1_object_set_int(store, 0x84);

  sec_hasp_asn1_object_set_str(store, 0x85);
  sec_hasp_asn1_object_set_str(store, 0x86);
  sec_hasp_asn1_object_set_str(store, 0x87);

  sec_hasp_asn1_object_set_int(store, 0x88);
  sec_hasp_asn1_object_set_int(store, 0x89);

  sec_hasp_asn1_object_set_time(store, 0x8A);

  sec_hasp_asn1_object_set_int(store, 0x8B);
  sec_hasp_asn1_object_set_int(store, 0x8C);
  sec_hasp_asn1_object_set_int(store, 0x8D);
  sec_hasp_asn1_object_set_int(store, 0x8E);
  sec_hasp_asn1_object_set_int(store, 0x8F);
  sec_hasp_asn1_object_set_int(store, 0x90);
  sec_hasp_asn1_object_set_int(store, 0x91);

  sec_hasp_asn1_decode(store, buffer, len);

  obj->oid = store->oid;

  sec_hasp_asn1_object_get_int(store, 0x80, &obj->pid);
  sec_hasp_asn1_object_get_int(store, 0x81, &obj->tid);
  sec_hasp_asn1_object_get_int(store, 0x82, &obj->hasp_uid);
  sec_hasp_asn1_object_get_int(store, 0x83, &obj->vendor_id);
  sec_hasp_asn1_object_get_int(store, 0x84, &obj->feature_id);

  sec_hasp_asn1_object_get_str(store, 0x85, &obj->username);
  sec_hasp_asn1_object_get_str(store, 0x86, &obj->machine_name);
  sec_hasp_asn1_object_get_str(store, 0x87, &obj->login_type);

  sec_hasp_asn1_object_get_int(store, 0x88, &obj->api_version_major);
  sec_hasp_asn1_object_get_int(store, 0x89, &obj->api_version_minor);

  sec_hasp_asn1_object_get_time(store, 0x8A, &obj->timestamp);

  sec_hasp_asn1_object_get_int(store, 0x8B, &obj->val_8b);
  sec_hasp_asn1_object_get_int(store, 0x8C, &obj->val_8c);
  sec_hasp_asn1_object_get_int(store, 0x8D, &obj->val_8d);
  sec_hasp_asn1_object_get_int(store, 0x8E, &obj->val_8e);
  sec_hasp_asn1_object_get_int(store, 0x8F, &obj->volume_serial);
  sec_hasp_asn1_object_get_int(store, 0x90, &obj->val_90);
  sec_hasp_asn1_object_get_int(store, 0x91, &obj->hasp_handle);
}