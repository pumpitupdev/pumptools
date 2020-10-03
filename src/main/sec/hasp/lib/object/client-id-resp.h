#ifndef SEC_HASP_OBJECT_CLIENT_ID_RESP_H
#define SEC_HASP_OBJECT_CLIENT_ID_RESP_H

#include <stdint.h>
#include <stdlib.h>

struct sec_hasp_object_client_id_resp {
    uint16_t oid;
    uint32_t status;
    uint32_t client_id;
};

struct sec_hasp_object_client_id_resp* sec_hasp_object_client_id_resp_alloc();

void sec_hasp_object_client_id_resp_free(struct sec_hasp_object_client_id_resp* obj);

size_t sec_hasp_object_client_id_resp_encode(struct sec_hasp_object_client_id_resp* obj, uint8_t* buffer,
    size_t len);

void sec_hasp_object_client_id_resp_decode(uint8_t* buffer, size_t len, struct sec_hasp_object_client_id_resp* obj);

#endif
