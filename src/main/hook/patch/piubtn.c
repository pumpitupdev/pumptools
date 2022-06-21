#define LOG_MODULE "patch-piubtn"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "capnhook/hook/lib.h"
#include "capnhook/hooklib/usb-emu.h"

#include "io/piubtn/defs.h"

#include "ptapi/io/piubtn.h"

#include "util/log.h"

static bool _patch_piubtn_enumerate(bool real_exists);
static enum cnh_result _patch_piubtn_open(void);
static enum cnh_result _patch_piubtn_reset(void);
static enum cnh_result _patch_piubtn_control_msg(
    int request_type,
    int request,
    int value,
    int index,
    struct cnh_iobuf *buffer,
    int timeout);
static void _patch_piubtn_close(void);

static enum cnh_result _patch_piubtn_process_inputs(struct cnh_iobuf *buffer);
static enum cnh_result _patch_piubtn_process_outputs(struct cnh_iobuf *buffer);

static ptapi_io_piubtn_ident_t _patch_piubtn_ptapi_io_piubtn_ident;
static ptapi_io_piubtn_open_t _patch_piubtn_ptapi_io_piubtn_open;
static ptapi_io_piubtn_close_t _patch_piubtn_ptapi_io_piubtn_close;
static ptapi_io_piubtn_recv_t _patch_piubtn_ptapi_io_piubtn_recv;
static ptapi_io_piubtn_send_t _patch_piubtn_ptapi_io_piubtn_send;
static ptapi_io_piubtn_get_input_t _patch_piubtn_ptapi_io_piubtn_get_input;
static ptapi_io_piubtn_set_output_t _patch_piubtn_ptapi_io_piubtn_set_output;

static uint32_t _patch_piubtn_poll_delay_ms;

static const struct cnh_usb_emu_virtdev_ep _patch_piubtn_virtdev = {
    .pid = PIUBTN_DRV_PID,
    .vid = PIUBTN_DRV_VID,
    .enumerate = _patch_piubtn_enumerate,
    .open = _patch_piubtn_open,
    .reset = _patch_piubtn_reset,
    .control_msg = _patch_piubtn_control_msg,
    .close = _patch_piubtn_close,
};

static void *_patch_piubtn_api_lib_handle;

void patch_piubtn_init(const char *piubtn_lib_path, uint32_t poll_delay_ms)
{
  _patch_piubtn_poll_delay_ms = poll_delay_ms;

  if (_patch_piubtn_poll_delay_ms > 0) {
    log_debug("Enabled poll delay ms: %d", poll_delay_ms);
  }

  if (!piubtn_lib_path) {
    log_die("No piubtn emulation library path specified");
  }

  /* Load piubtn library funcs */
  log_info("Loading piubtn api implementation %s", piubtn_lib_path);

  _patch_piubtn_api_lib_handle = cnh_lib_load(piubtn_lib_path);

  _patch_piubtn_ptapi_io_piubtn_ident =
      (ptapi_io_piubtn_ident_t) cnh_lib_get_func_addr_handle(
          _patch_piubtn_api_lib_handle, "ptapi_io_piubtn_ident");

  _patch_piubtn_ptapi_io_piubtn_open =
      (ptapi_io_piubtn_open_t) cnh_lib_get_func_addr_handle(
          _patch_piubtn_api_lib_handle, "ptapi_io_piubtn_open");
  _patch_piubtn_ptapi_io_piubtn_close =
      (ptapi_io_piubtn_close_t) cnh_lib_get_func_addr_handle(
          _patch_piubtn_api_lib_handle, "ptapi_io_piubtn_close");
  _patch_piubtn_ptapi_io_piubtn_recv =
      (ptapi_io_piubtn_recv_t) cnh_lib_get_func_addr_handle(
          _patch_piubtn_api_lib_handle, "ptapi_io_piubtn_recv");
  _patch_piubtn_ptapi_io_piubtn_send =
      (ptapi_io_piubtn_send_t) cnh_lib_get_func_addr_handle(
          _patch_piubtn_api_lib_handle, "ptapi_io_piubtn_send");
  _patch_piubtn_ptapi_io_piubtn_get_input =
      (ptapi_io_piubtn_get_input_t) cnh_lib_get_func_addr_handle(
          _patch_piubtn_api_lib_handle, "ptapi_io_piubtn_get_input");
  _patch_piubtn_ptapi_io_piubtn_set_output =
      (ptapi_io_piubtn_set_output_t) cnh_lib_get_func_addr_handle(
          _patch_piubtn_api_lib_handle, "ptapi_io_piubtn_set_output");

  cnh_usb_emu_add_virtdevep(&_patch_piubtn_virtdev);

  log_info("Initialized");
}

void patch_piubtn_shutdown(void)
{
}

static bool _patch_piubtn_enumerate(bool real_exists)
{
  if (real_exists) {
    log_info("Real PIUBTN exists but emulation enabled, blocking real");
  } else {
    log_info("Enumerating PIUBTN emulation");
  }

  return true;
}

static enum cnh_result _patch_piubtn_open(void)
{
  log_info("Opening PIUBTN: %s", _patch_piubtn_ptapi_io_piubtn_ident());

  if (!_patch_piubtn_ptapi_io_piubtn_open()) {
    log_error(
        "Opening api piubtn %s failed", _patch_piubtn_ptapi_io_piubtn_ident());
    return CNH_RESULT_OTHER_ERROR;
  }

  return CNH_RESULT_SUCCESS;
}

static enum cnh_result _patch_piubtn_reset(void)
{
  log_info("Resetting PIUBTN: %s", _patch_piubtn_ptapi_io_piubtn_ident());

  _patch_piubtn_ptapi_io_piubtn_close();

  if (!_patch_piubtn_ptapi_io_piubtn_open()) {
    log_error(
        "Resetting api piubtn %s failed",
        _patch_piubtn_ptapi_io_piubtn_ident());
    return CNH_RESULT_OTHER_ERROR;
  }

  return CNH_RESULT_SUCCESS;
}

static enum cnh_result _patch_piubtn_control_msg(
    int request_type,
    int request,
    int value,
    int index,
    struct cnh_iobuf *buffer,
    int timeout)
{
  if (_patch_piubtn_poll_delay_ms > 0) {
    usleep(_patch_piubtn_poll_delay_ms * 1000);
  }

  if (request_type == PIUBTN_DRV_USB_CTRL_TYPE_IN &&
      request == PIUBTN_DRV_USB_CTRL_REQUEST) {
    if (buffer->nbytes != PIUBTN_DRV_BUFFER_SIZE) {
      log_error("Invalid buffer size for ctrl in: %d", buffer->nbytes);
      return CNH_RESULT_INVALID_PARAMETER;
    }

    return _patch_piubtn_process_inputs(buffer);
  } else if (
      request_type == PIUBTN_DRV_USB_CTRL_TYPE_OUT &&
      request == PIUBTN_DRV_USB_CTRL_REQUEST) {
    if (buffer->nbytes != PIUBTN_DRV_BUFFER_SIZE) {
      log_error("Invalid buffer size for ctrl out: %d", buffer->nbytes);
      return CNH_RESULT_INVALID_PARAMETER;
    }

    return _patch_piubtn_process_outputs(buffer);
  } else {
    log_error("Invalid usb control request to PIUBTN: 0x%02X", request);
    return CNH_RESULT_INVALID_PARAMETER;
  }
}

static void _patch_piubtn_close(void)
{
  log_info("Closing PIUBTN: %s", _patch_piubtn_ptapi_io_piubtn_ident());

  _patch_piubtn_ptapi_io_piubtn_close();
}

static enum cnh_result _patch_piubtn_process_inputs(struct cnh_iobuf *buffer)
{
  struct ptapi_io_piubtn_inputs p1;
  struct ptapi_io_piubtn_inputs p2;

  if (!_patch_piubtn_ptapi_io_piubtn_recv()) {
    log_error(
        "Receiving inputs on api piubtn %s failed",
        _patch_piubtn_ptapi_io_piubtn_ident());
    return CNH_RESULT_OTHER_ERROR;
  }

  memset(&p1, 0, sizeof(p1));
  memset(&p2, 0, sizeof(p2));

  _patch_piubtn_ptapi_io_piubtn_get_input(0, &p1);
  _patch_piubtn_ptapi_io_piubtn_get_input(1, &p2);

  memset(buffer->bytes, 0, PIUBTN_DRV_BUFFER_SIZE);

  /*
     byte 0:
   bit 0: P1 back (red)
   bit 1: P1 left
   bit 2: P1 right
   bit 3: P1 start (green)
   bit 4: P2 back (red)
   bit 5: P2 left
   bit 6: P2 right
   bit 7: P2 start (green)
  */

  /* Player 1 */
  if (p1.back) {
    buffer->bytes[0] |= (1 << 0);
  }

  if (p1.left) {
    buffer->bytes[0] |= (1 << 1);
  }

  if (p1.right) {
    buffer->bytes[0] |= (1 << 2);
  }

  if (p1.start) {
    buffer->bytes[0] |= (1 << 3);
  }

  /* Player 2 */
  if (p2.back) {
    buffer->bytes[0] |= (1 << 4);
  }

  if (p2.left) {
    buffer->bytes[0] |= (1 << 5);
  }

  if (p2.right) {
    buffer->bytes[0] |= (1 << 6);
  }

  if (p2.start) {
    buffer->bytes[0] |= (1 << 7);
  }

  /* xor inputs because pullup active */
  for (int i = 0; i < PIUBTN_DRV_BUFFER_SIZE; i++) {
    buffer->bytes[i] ^= 0xFF;
  }

  buffer->pos = PIUBTN_DRV_BUFFER_SIZE;

  return CNH_RESULT_SUCCESS;
}

static enum cnh_result _patch_piubtn_process_outputs(struct cnh_iobuf *buffer)
{
  struct ptapi_io_piubtn_outputs p1;
  struct ptapi_io_piubtn_outputs p2;

  memset(&p1, 0, sizeof(p1));
  memset(&p2, 0, sizeof(p2));

  /*
   byte 0:
   bit 0: p2 start (green)
   bit 1: p2 right
   bit 2: p2 left
   bit 3: p2 back (red)
   bit 4: p1 start (green)
   bit 5: p1 right
   bit 6: p1 left
   bit 7: p1 back (red)
  */

  /* Player 2 */

  if (buffer->bytes[0] & (1 << 0)) {
    p2.start = true;
  }

  if (buffer->bytes[0] & (1 << 1)) {
    p2.right = true;
  }

  if (buffer->bytes[0] & (1 << 2)) {
    p2.left = true;
  }

  if (buffer->bytes[0] & (1 << 3)) {
    p2.back = true;
  }

  /* Player 1 */

  if (buffer->bytes[0] & (1 << 4)) {
    p1.start = true;
  }

  if (buffer->bytes[0] & (1 << 5)) {
    p1.right = true;
  }

  if (buffer->bytes[0] & (1 << 6)) {
    p1.left = true;
  }

  if (buffer->bytes[0] & (1 << 7)) {
    p1.back = true;
  }

  _patch_piubtn_ptapi_io_piubtn_set_output(0, &p1);
  _patch_piubtn_ptapi_io_piubtn_set_output(1, &p2);

  if (!_patch_piubtn_ptapi_io_piubtn_send()) {
    log_error(
        "Sending outputs on api piubtn %s failed",
        _patch_piubtn_ptapi_io_piubtn_ident());
    return CNH_RESULT_OTHER_ERROR;
  }

  buffer->pos = PIUBTN_DRV_BUFFER_SIZE;

  return CNH_RESULT_SUCCESS;
}