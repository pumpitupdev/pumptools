#ifndef SEC_HASP_OBJECT_CLIENT_ID_REQ_H
#define SEC_HASP_OBJECT_CLIENT_ID_REQ_H

#include <stdint.h>
#include <stdlib.h>

struct sec_hasp_object_client_id_req {
    uint16_t oid;
    uint32_t val_80;
    uint32_t api_version_major;
    uint32_t api_version_minor;
    uint32_t timestamp;
};

struct sec_hasp_object_client_id_req* sec_hasp_object_client_id_req_alloc();

void sec_hasp_object_client_id_req_free(struct sec_hasp_object_client_id_req* obj);

size_t sec_hasp_object_client_id_req_encode(struct sec_hasp_object_client_id_req* obj, uint8_t* buffer,
    size_t len);

void sec_hasp_object_client_id_req_decode(uint8_t* buffer, size_t len, struct sec_hasp_object_client_id_req* obj);

#endif
