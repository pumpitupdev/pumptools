#include <stdlib.h>
#include <string.h>

#include "util/log.h"
#include "util/mem.h"
#include "util/str.h"

#include "asn1.h"

static uint32_t
sec_hasp_asn1_decode_int(const uint8_t *buffer, size_t *offset, uint32_t blen);
static uint8_t *
sec_hasp_asn1_decode_data(const uint8_t *buffer, size_t *offset, uint32_t blen);
static char *
sec_hasp_asn1_decode_str(const uint8_t *buffer, size_t *offset, uint32_t blen);
static uint32_t
sec_hasp_asn1_decode_length(const uint8_t *buffer, size_t *offset);
static uint16_t sec_hasp_asn1_decode_tag(const uint8_t *buffer, size_t *offset);
static void
sec_hasp_asn1_encode_int(uint32_t val, uint8_t *buffer, size_t *offset);
static void sec_hasp_asn1_encode_data(
    const uint8_t *data, size_t len, uint8_t *buffer, size_t *offset);
static void
sec_hasp_asn1_encode_str(const char *str, uint8_t *buffer, size_t *offset);
static void
sec_hasp_asn1_encode_length(uint32_t len, uint8_t *buffer, size_t *offset);
static void
sec_hasp_asn1_encode_tag(uint16_t tag, uint8_t *buffer, size_t *offset);

struct sec_hasp_asn1_store *
sec_hasp_asn1_object_store_alloc(uint16_t oid, uint32_t size)
{
  struct sec_hasp_asn1_store *tmp = malloc(sizeof(struct sec_hasp_asn1_store));

  tmp->objects = malloc(sizeof(struct sec_hasp_asn1_object) * size);
  tmp->capacity = size;
  tmp->oid = oid;
  tmp->pos = 0;

  memset(tmp->objects, 0, sizeof(struct sec_hasp_asn1_object) * size);

  return tmp;
}

void sec_hasp_asn1_store_free(struct sec_hasp_asn1_store *store)
{
  for (size_t i = 0; i < store->pos; i++) {
    if (store->objects[i].type == SEC_HASP_ASN1_VT_STR) {
      free(store->objects[i].str);
    } else if (store->objects[i].type == SEC_HASP_ASN1_VT_DATA) {
      free(store->objects[i].data);
    }
  }

  free(store);
}

void sec_hasp_asn1_object_add_int(
    struct sec_hasp_asn1_store *store, uint16_t tag, uint32_t value)
{
  if (store->pos == store->capacity) {
    return;
  }

  store->objects[store->pos].tag = tag;
  store->objects[store->pos].type = SEC_HASP_ASN1_VT_INT;

  /* Determine length, considers padding for encoding later as well */
  // TODO not sure if this is correct, ask mario
  if (value <= 0xFF) {
    store->objects[store->pos].blen = 1;
  } else if (value <= 0xFFFF) {
    store->objects[store->pos].blen = 2;
  } else if (value <= 0xFFFFFF) {
    store->objects[store->pos].blen = 3;
  } else if (value <= 0x7FFFFFFF) {
    store->objects[store->pos].blen = 4;
  } else {
    store->objects[store->pos].blen = 5;
  }

  store->objects[store->pos].int_val = value;
  store->pos++;
}

void sec_hasp_asn1_object_add_str(
    struct sec_hasp_asn1_store *store, uint16_t tag, const char *str)
{
  if (store->pos == store->capacity) {
    return;
  }

  store->objects[store->pos].tag = tag;
  store->objects[store->pos].type = SEC_HASP_ASN1_VT_STR;

  /* Store len + null terminator */
  store->objects[store->pos].blen = strlen(str) + 1;
  store->objects[store->pos].str = util_str_dup(str);
  store->pos++;
}

void sec_hasp_asn1_object_add_time(
    struct sec_hasp_asn1_store *store, uint16_t tag, uint32_t time)
{
  if (store->pos == store->capacity) {
    return;
  }

  store->objects[store->pos].tag = tag;
  store->objects[store->pos].type = SEC_HASP_ASN1_VT_TIME;

  store->objects[store->pos].blen = 5;
  store->objects[store->pos].time = time;
  store->pos++;
}

void sec_hasp_asn1_object_add_data(
    struct sec_hasp_asn1_store *store,
    uint16_t tag,
    const uint8_t *data,
    size_t len)
{
  if (store->pos == store->capacity) {
    return;
  }

  store->objects[store->pos].tag = tag;
  store->objects[store->pos].type = SEC_HASP_ASN1_VT_DATA;

  /* Store len + null terminator */
  store->objects[store->pos].blen = len;
  store->objects[store->pos].data = util_xmalloc(len);
  memcpy(store->objects[store->pos].data, data, len);

  store->pos++;
}

void sec_hasp_asn1_object_set_int(
    struct sec_hasp_asn1_store *store, uint16_t tag)
{
  store->objects[store->pos].tag = tag;
  store->objects[store->pos].type = SEC_HASP_ASN1_VT_INT;
  store->pos++;
}

void sec_hasp_asn1_object_set_str(
    struct sec_hasp_asn1_store *store, uint16_t tag)
{
  store->objects[store->pos].tag = tag;
  store->objects[store->pos].type = SEC_HASP_ASN1_VT_STR;
  store->pos++;
}

void sec_hasp_asn1_object_set_time(
    struct sec_hasp_asn1_store *store, uint16_t tag)
{
  store->objects[store->pos].tag = tag;
  store->objects[store->pos].type = SEC_HASP_ASN1_VT_TIME;
  store->pos++;
}

void sec_hasp_asn1_object_set_data(
    struct sec_hasp_asn1_store *store, uint16_t tag)
{
  store->objects[store->pos].tag = tag;
  store->objects[store->pos].type = SEC_HASP_ASN1_VT_DATA;
  store->pos++;
}

bool sec_hasp_asn1_object_get_int(
    struct sec_hasp_asn1_store *store, uint16_t tag, uint32_t *ret_value)
{
  for (size_t i = 0; i < store->pos; i++) {
    if (store->objects[i].tag == tag) {
      if (store->objects[i].type != SEC_HASP_ASN1_VT_INT) {
        log_error("Type of tag %X is not int", tag);
        return false;
      } else {
        *ret_value = store->objects[i].int_val;
        return true;
      }
    }
  }

  return false;
}

bool sec_hasp_asn1_object_get_str(
    struct sec_hasp_asn1_store *store, uint16_t tag, char **ret_value)
{
  for (size_t i = 0; i < store->pos; i++) {
    if (store->objects[i].tag == tag) {
      if (store->objects[i].type != SEC_HASP_ASN1_VT_STR) {
        log_error("Type of tag %X is not str", tag);
        return false;
      } else {
        *ret_value = util_str_dup(store->objects[i].str);
        return true;
      }
    }
  }

  return false;
}

bool sec_hasp_asn1_object_get_time(
    struct sec_hasp_asn1_store *store, uint16_t tag, uint32_t *ret_value)
{
  for (size_t i = 0; i < store->pos; i++) {
    if (store->objects[i].tag == tag) {
      if (store->objects[i].type != SEC_HASP_ASN1_VT_TIME) {
        log_error("Type of tag %X is not time", tag);
        return false;
      } else {
        *ret_value = store->objects[i].int_val;
        return true;
      }
    }
  }

  return false;
}

bool sec_hasp_asn1_object_get_data(
    struct sec_hasp_asn1_store *store,
    uint16_t tag,
    uint8_t **ret_data,
    size_t *ret_len)
{
  for (size_t i = 0; i < store->pos; i++) {
    if (store->objects[i].tag == tag) {
      if (store->objects[i].type != SEC_HASP_ASN1_VT_DATA) {
        log_error("Type of tag %X is not str", tag);
        return false;
      } else {
        *ret_len = store->objects[i].blen;
        *ret_data = util_xmalloc(*ret_len);
        memcpy(*ret_data, store->objects[i].data, *ret_len);
        return true;
      }
    }
  }

  return false;
}

size_t sec_hasp_asn1_encode(
    struct sec_hasp_asn1_store *store, uint8_t *buffer, size_t len)
{
  uint8_t buffer_tmp[32768];
  size_t offset = 0;

  /* we need to know the size of the payload to encode the first length field,
   * encode all objects first */
  for (size_t i = 0; i < store->pos; i++) {
    sec_hasp_asn1_encode_tag(store->objects[i].tag, buffer_tmp, &offset);
    sec_hasp_asn1_encode_length(store->objects[i].blen, buffer_tmp, &offset);

    switch (store->objects[i].type) {
      case SEC_HASP_ASN1_VT_INVALID:
        log_error("Invalid object type");
        break;

      case SEC_HASP_ASN1_VT_TIME:
        buffer_tmp[offset++] = 0x00;
        sec_hasp_asn1_encode_int(store->objects[i].time, buffer_tmp, &offset);
        break;

      case SEC_HASP_ASN1_VT_INT:
        sec_hasp_asn1_encode_int(store->objects[i].time, buffer_tmp, &offset);
        break;

      case SEC_HASP_ASN1_VT_STR:
        sec_hasp_asn1_encode_str(store->objects[i].str, buffer_tmp, &offset);
        break;

      case SEC_HASP_ASN1_VT_DATA:
        sec_hasp_asn1_encode_data(
            store->objects[i].data,
            store->objects[i].blen,
            buffer_tmp,
            &offset);
        break;

      default:
        log_error("Unknown object type %d", store->objects[i].type);
        break;
    }
  }

  size_t offset_out = 0;

  sec_hasp_asn1_encode_tag(store->oid, buffer, &offset_out);
  sec_hasp_asn1_encode_length(offset, buffer, &offset_out);

  if (offset > len) {
    log_error("Target buffer too small: %d < %d", len, offset + offset_out);
  } else {
    memcpy(&buffer[offset_out], buffer_tmp, offset);
  }

  return offset + offset_out;
}

void sec_hasp_asn1_decode(
    struct sec_hasp_asn1_store *store, const uint8_t *buffer, size_t len)
{
  size_t offset = 0;

  store->oid = sec_hasp_asn1_decode_tag(buffer, &offset);
  sec_hasp_asn1_decode_length(buffer, &offset);

  while (offset < len) {
    uint16_t tag = sec_hasp_asn1_decode_tag(buffer, &offset);
    uint32_t obj_len = sec_hasp_asn1_decode_length(buffer, &offset);

    if (tag <= 0x7F || tag >= 0x9F) {
      log_error("Found invalid object tag: %X", tag);
      return;
    }

    struct sec_hasp_asn1_object *obj = NULL;

    /* find object that matches the tag and type */
    for (size_t i = 0; i < store->pos; i++) {
      if (store->objects[i].tag == tag) {
        obj = &store->objects[i];
        break;
      }
    }

    if (!obj) {
      log_error("No object in store that matches tag %X", tag);
      return;
    }

    obj->blen = obj_len;

    switch (obj->type) {
      case SEC_HASP_ASN1_VT_INVALID:
        log_error("Invalid object type specified for tag %X", obj->tag);
        break;

      case SEC_HASP_ASN1_VT_TIME:
      case SEC_HASP_ASN1_VT_INT:
        obj->int_val = sec_hasp_asn1_decode_int(buffer, &offset, obj->blen);
        break;

      case SEC_HASP_ASN1_VT_STR:
        obj->str = sec_hasp_asn1_decode_str(buffer, &offset, obj->blen);
        break;

      case SEC_HASP_ASN1_VT_DATA:
        obj->data = sec_hasp_asn1_decode_data(buffer, &offset, obj->blen);
        break;

      default:
        log_error("Unknown object type for tag %X", obj->tag);
        break;
    }
  }
}

static uint32_t
sec_hasp_asn1_decode_int(const uint8_t *buffer, size_t *offset, uint32_t blen)
{
  uint32_t data = 0;

  switch (blen) {
    case 1:
      data = buffer[(*offset)++];
      break;

    case 2:
      data |= buffer[(*offset)++] << 8;
      data |= buffer[(*offset)++];
      break;

    case 3:
      data |= buffer[(*offset)++] << 16;
      data |= buffer[(*offset)++] << 8;
      data |= buffer[(*offset)++];
      break;

    case 4:
      data |= buffer[(*offset)++] << 24;
      data |= buffer[(*offset)++] << 16;
      data |= buffer[(*offset)++] << 8;
      data |= buffer[(*offset)++];
      break;

    case 5:
      /* skip first byte? */
      (*offset)++;
      data |= buffer[(*offset)++] << 24;
      data |= buffer[(*offset)++] << 16;
      data |= buffer[(*offset)++] << 8;
      data |= buffer[(*offset)++];
      break;

    default:
      // TODO invalid len?
      break;
  }

  return data;
}

static uint8_t *
sec_hasp_asn1_decode_data(const uint8_t *buffer, size_t *offset, uint32_t blen)
{
  uint8_t *data = util_xmalloc(sizeof(uint8_t) * blen);

  memcpy(data, buffer + *offset, blen);
  (*offset) += blen;

  return data;
}

static char *
sec_hasp_asn1_decode_str(const uint8_t *buffer, size_t *offset, uint32_t blen)
{
  /* len already includes null terminator */
  char *str = util_xmalloc(sizeof(char) * blen);

  memcpy(str, buffer + *offset, blen);
  (*offset) += blen;

  return str;
}

static uint32_t
sec_hasp_asn1_decode_length(const uint8_t *buffer, size_t *offset)
{
  uint32_t len;

  len = buffer[(*offset)++];

  if (len == 0x81) {
    len = buffer[(*offset)++];
  } else if (len == 0x82) {
    /* Big endian */
    len = buffer[(*offset)++] << 8;
    len |= buffer[(*offset)++];
  }

  return len;
}

static uint16_t sec_hasp_asn1_decode_tag(const uint8_t *buffer, size_t *offset)
{
  uint16_t tag;

  tag = buffer[(*offset)++];

  if (tag == 0x7F) {
    /* Big endian */
    tag = tag << 8;
    tag |= buffer[(*offset)++];
  }

  return tag;
}

static void
sec_hasp_asn1_encode_int(uint32_t val, uint8_t *buffer, size_t *offset)
{
  uint8_t tmp[8];
  size_t tmp_pos = 0;

  if (val <= 0xFF) {
    tmp[tmp_pos++] = (uint8_t) val;
  } else if (val <= 0xFFFF) {
    /* Big endian */
    tmp[tmp_pos++] = (uint8_t)(val >> 8);
    tmp[tmp_pos++] = (uint8_t) val;
  } else if (val <= 0xFFFFFF) {
    /* Big endian */
    tmp[tmp_pos++] = (uint8_t)(val >> 16);
    tmp[tmp_pos++] = (uint8_t)(val >> 8);
    tmp[tmp_pos++] = (uint8_t) val;
  } else if (val <= 0xFFFFFFFF) {
    /* Big endian */
    tmp[tmp_pos++] = (uint8_t)(val >> 24);
    tmp[tmp_pos++] = (uint8_t)(val >> 16);
    tmp[tmp_pos++] = (uint8_t)(val >> 8);
    tmp[tmp_pos++] = (uint8_t) val;
  } else if (val <= 0xFFFFFFFFFFFFFFFF) {
    // TODO quad words not supported in decode?
  }

  /* Deal with padding */
  if (tmp[0] > 0x7F) {
    buffer[(*offset)++] = 0x00;
  }

  memcpy(buffer + *offset, tmp, tmp_pos);
  (*offset) += tmp_pos;
}

static void sec_hasp_asn1_encode_data(
    const uint8_t *data, size_t len, uint8_t *buffer, size_t *offset)
{
  memcpy(buffer + *offset, data, len);
  (*offset) += len;
}

static void
sec_hasp_asn1_encode_str(const char *str, uint8_t *buffer, size_t *offset)
{
  size_t len = strlen(str);

  memcpy(buffer + *offset, str, len);
  (*offset) += len;

  /* Add null terminator */
  buffer[(*offset)++] = 0x00;
}

static void
sec_hasp_asn1_encode_length(uint32_t len, uint8_t *buffer, size_t *offset)
{
  if (len <= 0xF7) {
    buffer[(*offset)++] = (uint8_t) len;
  } else if (len <= 0xFF) {
    buffer[(*offset)++] = 0x81;
    buffer[(*offset)++] = (uint8_t) len;
  } else if (len <= 0xFFFF) {
    buffer[(*offset)++] = 0x82;
    /* Big endian */
    buffer[(*offset)++] = (uint8_t)(len >> 8);
    buffer[(*offset)++] = (uint8_t) len;
  } else if (len <= 0xFFFF) {
    buffer[(*offset)++] = 0x82;
    /* Big endian */
    buffer[(*offset)++] = (uint8_t)(len >> 16);
    buffer[(*offset)++] = (uint8_t)(len >> 8);
    buffer[(*offset)++] = (uint8_t) len;
  } else if (len <= 0xFFFFFFFF) {
    buffer[(*offset)++] = 0x84;
    /* Big endian */
    buffer[(*offset)++] = (uint8_t)(len >> 24);
    buffer[(*offset)++] = (uint8_t)(len >> 16);
    buffer[(*offset)++] = (uint8_t)(len >> 8);
    buffer[(*offset)++] = (uint8_t) len;
  }
}

static void
sec_hasp_asn1_encode_tag(uint16_t tag, uint8_t *buffer, size_t *offset)
{
  if (tag > 0xFF) {
    /* Big endian */
    buffer[(*offset)++] = (uint8_t)(tag >> 8);
    buffer[(*offset)++] = (uint8_t)(tag);
  } else {
    buffer[(*offset)++] = (uint8_t)(tag);
  }
}