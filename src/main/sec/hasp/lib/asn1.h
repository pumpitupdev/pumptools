#ifndef SEC_HASP_ASN1_H
#define SEC_HASP_ASN1_H

#include <stdint.h>
#include <stdlib.h>

#include "util/list.h"

enum sec_hasp_asn1_value_type {
  SEC_HASP_ASN1_VT_INVALID = 0,
  SEC_HASP_ASN1_VT_INT = 1,
  SEC_HASP_ASN1_VT_STR = 2,
  SEC_HASP_ASN1_VT_TIME = 3,
  SEC_HASP_ASN1_VT_DATA = 4,
};

struct sec_hasp_asn1_object {
  uint16_t tag;
  uint32_t blen;
  enum sec_hasp_asn1_value_type type;

  union {
    uint32_t time;
    uint32_t int_val;
    uint8_t *data;
    char *str;
  };
};

struct sec_hasp_asn1_store {
  uint16_t oid;
  size_t capacity;
  size_t pos;
  struct sec_hasp_asn1_object *objects;
};

struct sec_hasp_asn1_store *
sec_hasp_asn1_object_store_alloc(uint16_t oid, uint32_t size);

void sec_hasp_asn1_store_free(struct sec_hasp_asn1_store *store);

void sec_hasp_asn1_object_add_int(
    struct sec_hasp_asn1_store *store, uint16_t tag, uint32_t value);

void sec_hasp_asn1_object_add_str(
    struct sec_hasp_asn1_store *store, uint16_t tag, const char *str);

void sec_hasp_asn1_object_add_time(
    struct sec_hasp_asn1_store *store, uint16_t tag, uint32_t time);

void sec_hasp_asn1_object_add_data(
    struct sec_hasp_asn1_store *store,
    uint16_t tag,
    const uint8_t *data,
    size_t len);

void sec_hasp_asn1_object_set_int(
    struct sec_hasp_asn1_store *store, uint16_t tag);

void sec_hasp_asn1_object_set_str(
    struct sec_hasp_asn1_store *store, uint16_t tag);

void sec_hasp_asn1_object_set_time(
    struct sec_hasp_asn1_store *store, uint16_t tag);

void sec_hasp_asn1_object_set_data(
    struct sec_hasp_asn1_store *store, uint16_t tag);

bool sec_hasp_asn1_object_get_int(
    struct sec_hasp_asn1_store *store, uint16_t tag, uint32_t *ret_value);

bool sec_hasp_asn1_object_get_str(
    struct sec_hasp_asn1_store *store, uint16_t tag, char **ret_value);

bool sec_hasp_asn1_object_get_time(
    struct sec_hasp_asn1_store *store, uint16_t tag, uint32_t *ret_value);

bool sec_hasp_asn1_object_get_data(
    struct sec_hasp_asn1_store *store,
    uint16_t tag,
    uint8_t **ret_data,
    size_t *ret_len);

size_t sec_hasp_asn1_encode(
    struct sec_hasp_asn1_store *store, uint8_t *buffer, size_t len);

void sec_hasp_asn1_decode(
    struct sec_hasp_asn1_store *store, const uint8_t *buffer, size_t len);

#endif
