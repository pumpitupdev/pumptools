#include "sec/hasp/lib/asn1.h"
#include "sec/hasp/lib/const.h"

#include "util/mem.h"

#include "client-id-resp.h"

struct sec_hasp_object_client_id_resp* sec_hasp_object_client_id_resp_alloc()
{
    struct sec_hasp_object_client_id_resp* obj = util_xmalloc(sizeof(struct sec_hasp_object_client_id_resp));

    obj->oid = SEC_HASP_CONST_OID_CLIENTID_RESP;
    obj->status = 0;
    obj->client_id = 0;

    return obj;
}

void sec_hasp_object_client_id_resp_free(struct sec_hasp_object_client_id_resp* obj)
{
    free(obj);
}

size_t sec_hasp_object_client_id_resp_encode(struct sec_hasp_object_client_id_resp* obj, uint8_t* buffer,
        size_t len)
{
    struct sec_hasp_asn1_store* store = sec_hasp_asn1_object_store_alloc(obj->oid, 4);

    sec_hasp_asn1_object_add_int(store, 0x80, obj->status);
    sec_hasp_asn1_object_add_int(store, 0x81, obj->client_id);

    return sec_hasp_asn1_encode(store, buffer, len);
}

void sec_hasp_object_client_id_resp_decode(uint8_t* buffer, size_t len, struct sec_hasp_object_client_id_resp* obj)
{
    struct sec_hasp_asn1_store* store = sec_hasp_asn1_object_store_alloc(obj->oid, 2);

    sec_hasp_asn1_object_set_int(store, 0x80);
    sec_hasp_asn1_object_set_int(store, 0x81);

    sec_hasp_asn1_decode(store, buffer, len);

    obj->oid = store->oid;

    sec_hasp_asn1_object_get_int(store, 0x80, &obj->status);
    sec_hasp_asn1_object_get_int(store, 0x81, &obj->client_id);
}