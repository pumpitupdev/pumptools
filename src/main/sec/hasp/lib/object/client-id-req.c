#include <time.h>

#include "sec/hasp/lib/asn1.h"
#include "sec/hasp/lib/const.h"

#include "util/mem.h"

#include "client-id-req.h"

struct sec_hasp_object_client_id_req *sec_hasp_object_client_id_req_alloc()
{
  struct sec_hasp_object_client_id_req *obj =
      util_xmalloc(sizeof(struct sec_hasp_object_client_id_req));

  obj->oid = SEC_HASP_CONST_OID_CLIENTID_REQ;
  obj->val_80 = 0;
  obj->api_version_major = 0;
  obj->api_version_minor = 0;
  obj->timestamp = 0;

  return obj;
}

void sec_hasp_object_client_id_req_free(
    struct sec_hasp_object_client_id_req *obj)
{
  free(obj);
}

size_t sec_hasp_object_client_id_req_encode(
    struct sec_hasp_object_client_id_req *obj, uint8_t *buffer, size_t len)
{
  struct sec_hasp_asn1_store *store =
      sec_hasp_asn1_object_store_alloc(obj->oid, 4);

  /* populate right before encoding */
  obj->timestamp = (uint32_t) time(NULL);

  sec_hasp_asn1_object_add_int(store, 0x80, obj->val_80);
  sec_hasp_asn1_object_add_int(store, 0x81, obj->api_version_major);
  sec_hasp_asn1_object_add_int(store, 0x82, obj->api_version_minor);
  sec_hasp_asn1_object_add_time(store, 0x84, obj->timestamp);

  return sec_hasp_asn1_encode(store, buffer, len);
}

void sec_hasp_object_client_id_req_decode(
    uint8_t *buffer, size_t len, struct sec_hasp_object_client_id_req *obj)
{
  struct sec_hasp_asn1_store *store =
      sec_hasp_asn1_object_store_alloc(obj->oid, 4);

  sec_hasp_asn1_object_set_int(store, 0x80);
  sec_hasp_asn1_object_set_int(store, 0x81);
  sec_hasp_asn1_object_set_int(store, 0x82);
  sec_hasp_asn1_object_set_time(store, 0x84);

  sec_hasp_asn1_decode(store, buffer, len);

  obj->oid = store->oid;

  sec_hasp_asn1_object_get_int(store, 0x80, &obj->val_80);
  sec_hasp_asn1_object_get_int(store, 0x81, &obj->api_version_major);
  sec_hasp_asn1_object_get_int(store, 0x82, &obj->api_version_minor);
  sec_hasp_asn1_object_get_time(store, 0x84, &obj->timestamp);
}