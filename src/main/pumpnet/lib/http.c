#define LOG_MODULE "http"

#include <curl/curl.h>
#include <stdint.h>

#include "util/base64.h"
#include "util/fs.h"
#include "util/log.h"
#include "util/mem.h"
#include "util/str.h"

#define MEDIA_TYPE "text/plain"
#define RECV_BUFFER_SIZE 1024 * 512
#define CLIENT_CRT "/client-crt.pem"
#define CLIENT_KEY "/client-key.pem"
#define CA_BUNDLE_CRT "/ca-bundle-crt.pem"

struct pumpnet_lib_http_buffer {
  void *data;
  size_t size;
  size_t pos;
};

static size_t _pumpnet_lib_http_curl_cb_write_data(
    void *ptr, size_t size, size_t nmemb, void *ctx)
{
  struct pumpnet_lib_http_buffer *buffer;

  buffer = (struct pumpnet_lib_http_buffer *) ctx;

  if (buffer->pos == buffer->size) {
    return 0;
  }

  if (buffer->pos + size * nmemb >= buffer->size) {
    log_error(
        "Buffer overflow, pos %d, size %d | size %d, nmemb %d",
        buffer->pos,
        buffer->size,
        size,
        nmemb);
    return 0;
  }

  memcpy(buffer->data + buffer->pos, ptr, size * nmemb);
  buffer->pos += size * nmemb;

  return size * nmemb;
}

static size_t _pumpnet_lib_http_curl_cb_read_data(
    void *ptr, size_t size, size_t nmemb, void *ctx)
{
  struct pumpnet_lib_http_buffer *buffer;
  size_t max_size;
  size_t read;

  max_size = size * nmemb;

  buffer = (struct pumpnet_lib_http_buffer *) ctx;

  if (buffer->size == buffer->pos) {
    return 0;
  }

  if (buffer->size - buffer->pos < max_size) {
    read = buffer->size - buffer->pos;
  } else {
    read = max_size;
  }

  memcpy(ptr, buffer->data + buffer->pos, read);

  buffer->pos += read;

  return read;
}

int _pumpnet_libcurl_debug_callback(
    CURL *handle, curl_infotype type, char *data, size_t size, void *userptr)
{
  char *null_term_buffer;

  switch (type) {
    case CURLINFO_TEXT:
      log_debug("[CURL]: %s", data);
      break;

    case CURLINFO_HEADER_IN:
    case CURLINFO_HEADER_OUT:
      null_term_buffer = util_xmalloc(size + 1);
      memcpy(null_term_buffer, data, size);
      null_term_buffer[size] = '\0';

      log_debug("[CURL]: %d (%p), len %ld:\n%s", type, handle, size, data);

      free(null_term_buffer);
      break;
    case CURLINFO_DATA_IN:
    case CURLINFO_DATA_OUT:
    case CURLINFO_SSL_DATA_OUT:
    case CURLINFO_SSL_DATA_IN:
      log_debug("[CURL]: %d (%p), len %ld", type, handle, size);
      break;
    default:
      log_die_illegal_state();
  }

  return 0;
}

static char *pumpnet_lib_http_client_crt_path;
static char *pumpnet_lib_http_client_key_path;
static char *pumpnet_lib_http_ca_bundle_crt_path;
static bool pumpnet_lib_http_verbose_debug_log;

void pumpnet_lib_http_init(const char *cert_dir_path, bool verbose_debug_log)
{
  if (cert_dir_path) {
    char *absolute_path = util_fs_get_abs_path(cert_dir_path);

    pumpnet_lib_http_client_crt_path =
        util_str_merge(absolute_path, CLIENT_CRT);
    pumpnet_lib_http_client_key_path =
        util_str_merge(absolute_path, CLIENT_KEY);
    pumpnet_lib_http_ca_bundle_crt_path =
        util_str_merge(absolute_path, CA_BUNDLE_CRT);

    free(absolute_path);
  }

  pumpnet_lib_http_verbose_debug_log = verbose_debug_log;

  log_info(
      "Initialized, client crt %s, client key %s, ca bundle crt %s, verbose "
      "debug log %d",
      pumpnet_lib_http_client_crt_path,
      pumpnet_lib_http_client_key_path,
      pumpnet_lib_http_ca_bundle_crt_path,
      verbose_debug_log);
}

void pumpnet_lib_http_shutdown()
{
  if (pumpnet_lib_http_client_crt_path) {
    free(pumpnet_lib_http_client_crt_path);
  }

  if (pumpnet_lib_http_client_key_path) {
    free(pumpnet_lib_http_client_key_path);
  }

  if (pumpnet_lib_http_ca_bundle_crt_path) {
    free(pumpnet_lib_http_ca_bundle_crt_path);
  }
}

bool pumpnet_lib_http_get_put(
    uint64_t trace_id,
    const char *address,
    void *send_data,
    size_t send_size,
    void *recv_data,
    size_t recv_size,
    uint32_t *http_code,
    bool is_post)
{
  log_assert(address);
  log_assert(send_data);
  log_assert(recv_data);
  log_assert(http_code);

  CURL *curl_handle;
  CURLcode res;
  struct pumpnet_lib_http_buffer send_buffer;
  struct pumpnet_lib_http_buffer recv_buffer;
  struct curl_slist *host = NULL;

  *http_code = 0;

  curl_handle = curl_easy_init();

  if (!curl_handle) {
    log_error("[%llX] Initializing curl backend failed", trace_id);
    return false;
  }

  send_buffer.data =
      util_base64_encode(send_data, send_size, &send_buffer.size);
  send_buffer.pos = 0;

  recv_buffer.data = util_xmalloc(RECV_BUFFER_SIZE);
  recv_buffer.size = RECV_BUFFER_SIZE;
  recv_buffer.pos = 0;

  if (is_post) {
    curl_easy_setopt(curl_handle, CURLOPT_POST, 1L);
  } else {
    curl_easy_setopt(curl_handle, CURLOPT_POST, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "GET");
  }

  if (pumpnet_lib_http_verbose_debug_log) {
    curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1);
    curl_easy_setopt(
        curl_handle, CURLOPT_DEBUGFUNCTION, _pumpnet_libcurl_debug_callback);
  }

  if (pumpnet_lib_http_client_crt_path) {
    host = curl_slist_append(NULL, "pumpnet:443:185.41.243.94");
    curl_easy_setopt(curl_handle, CURLOPT_RESOLVE, host);
    curl_easy_setopt(
        curl_handle, CURLOPT_SSLCERT, pumpnet_lib_http_client_crt_path);
    curl_easy_setopt(
        curl_handle, CURLOPT_SSLKEY, pumpnet_lib_http_client_key_path);
    curl_easy_setopt(curl_handle, CURLOPT_SSLKEYTYPE, "PEM");
    curl_easy_setopt(
        curl_handle, CURLOPT_CAINFO, pumpnet_lib_http_ca_bundle_crt_path);
    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 1);
  }

  // It seems like some libcurl revisions cut off the content after processing
  // it using the provided CURLOPT_READFUNCTION if the size is not force set
  // like this
  curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, send_buffer.size);

  curl_easy_setopt(curl_handle, CURLOPT_URL, address);
  curl_easy_setopt(
      curl_handle, CURLOPT_READFUNCTION, _pumpnet_lib_http_curl_cb_read_data);
  curl_easy_setopt(curl_handle, CURLOPT_READDATA, &send_buffer);
  curl_easy_setopt(
      curl_handle, CURLOPT_WRITEFUNCTION, _pumpnet_lib_http_curl_cb_write_data);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &recv_buffer);

  struct curl_slist *hs = NULL;
  hs = curl_slist_append(hs, "Content-Type: " MEDIA_TYPE);
  curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, hs);

  log_debug(
      "[%llX][%s] %s %d %d",
      trace_id,
      address,
      is_post ? "POST" : "GET",
      send_size,
      recv_size);

  res = curl_easy_perform(curl_handle);

  curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, http_code);

  log_debug(
      "[%llX][%s] %s: %d (%d %d)",
      trace_id,
      address,
      is_post ? "POST" : "GET",
      *http_code,
      recv_buffer.pos,
      send_buffer.pos);

  if (host) {
    curl_slist_free_all(host);
  }

  curl_slist_free_all(hs);

  curl_easy_cleanup(curl_handle);

  if (res != CURLE_OK) {
    log_error(
        "[%llX][%s][%d] Performing curl request failed: %s",
        trace_id,
        address,
        is_post,
        curl_easy_strerror(res));

    free(send_buffer.data);
    free(recv_buffer.data);
    return false;
  }

  if (send_buffer.pos != send_buffer.size) {
    log_error(
        "[%llX][%s][%d] Invalid send data size: %d != %d",
        trace_id,
        address,
        is_post,
        send_buffer.pos,
        send_buffer.size);

    free(send_buffer.data);
    free(recv_buffer.data);
    return false;
  }

  free(send_buffer.data);

  size_t recv_size_decoded;
  uint8_t *tmp_recv_decoded =
      util_base64_decode(recv_buffer.data, recv_buffer.pos, &recv_size_decoded);

  free(recv_buffer.data);

  if (recv_size_decoded != recv_size) {
    log_error(
        "[%llX][%s][%d] Invalid recv data size: %d != %d",
        trace_id,
        address,
        is_post,
        recv_size_decoded,
        recv_size);

    free(tmp_recv_decoded);
    return false;
  }

  memcpy(recv_data, tmp_recv_decoded, recv_size);
  free(tmp_recv_decoded);

  return true;
}