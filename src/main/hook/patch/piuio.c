#define LOG_MODULE "patch-piuio"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "capnhook/hooklib/usb-emu.h"

#include "io/piuio/defs.h"

#include "ptapi/io/piuio.h"
#include "ptapi/io/piuio/util/lib.h"

#include "util/log.h"

// Enable this to get a detailed "call trace" of reads/writes and updates
// for debugging purpose
// #define PATCH_PIUIO_CALL_TRACE

static bool _patch_piuio_enumerate(bool real_exists);
static enum cnh_result _patch_piuio_open(void);
static enum cnh_result _patch_piuio_reset(void);
static enum cnh_result _patch_piuio_control_msg(
    int request_type,
    int request,
    int value,
    int index,
    struct cnh_iobuf *buffer,
    int timeout);
static void _patch_piuio_close(void);

static void _patch_piuio_read_inputs_to_buffer(struct cnh_iobuf *buffer);
static void _patch_piuio_read_outputs_from_buffer(struct cnh_iobuf *buffer);
static enum ptapi_io_piuio_sensor_group
_patch_piuio_get_sensor_group_from_buffer(struct cnh_iobuf *buffer);

static const struct cnh_usb_emu_virtdev_ep _patch_piuio_virtdev = {
    .pid = PIUIO_DRV_PID,
    .vid = PIUIO_DRV_VID,
    .enumerate = _patch_piuio_enumerate,
    .open = _patch_piuio_open,
    .reset = _patch_piuio_reset,
    .control_msg = _patch_piuio_control_msg,
    .close = _patch_piuio_close,
};

static uint32_t _patch_piuio_poll_delay_ms;
static struct ptapi_io_piuio_api _patch_piuio_api;
static enum ptapi_io_piuio_sensor_group _patch_piuio_sensor_group;

// Improved input state management 
static struct ptapi_io_piuio_pad_inputs _patch_piuio_pad_state[2][PTAPI_IO_PIUIO_SENSOR_GROUP_NUM];
static struct ptapi_io_piuio_sys_inputs _patch_piuio_sys_state;
static bool _patch_piuio_state_initialized = false;

void patch_piuio_init(const char *piuio_lib_path, uint32_t poll_delay_ms)
{
  _patch_piuio_poll_delay_ms = poll_delay_ms;

  if (_patch_piuio_poll_delay_ms > 0) {
    log_debug("Enabled poll delay ms: %d", poll_delay_ms);
  }

  if (!piuio_lib_path) {
    log_die("No piuio emulation library path specified");
  }

  /* Load piuio library funcs */
  log_info("Loading piuio api implementation %s", piuio_lib_path);

  if (!ptapi_io_piuio_util_lib_load(piuio_lib_path, &_patch_piuio_api)) {
    log_error("Loading PIUIO API from %s failed", piuio_lib_path);
    return;
  }

  // Initialize all input state
  memset(_patch_piuio_pad_state, 0, sizeof(_patch_piuio_pad_state));
  memset(&_patch_piuio_sys_state, 0, sizeof(_patch_piuio_sys_state));
  _patch_piuio_state_initialized = false;

  cnh_usb_emu_add_virtdevep(&_patch_piuio_virtdev);

  log_info("Initialized with improved buffer handling");
}

void patch_piuio_shutdown(void)
{
}

static bool _patch_piuio_enumerate(bool real_exists)
{
  if (real_exists) {
    log_info("Real PIUIO exists but emulation enabled, blocking real");
  } else {
    log_info("Enumerating PIUIO emulation");
  }

  return true;
}

static enum cnh_result _patch_piuio_open(void)
{
  log_info("Opening PIUIO: %s", _patch_piuio_api.ident());

  if (!_patch_piuio_api.open()) {
    log_error("Opening api piuio %s failed", _patch_piuio_api.ident());
    return CNH_RESULT_OTHER_ERROR;
  }

  // Reset state on device open
  memset(_patch_piuio_pad_state, 0, sizeof(_patch_piuio_pad_state));
  memset(&_patch_piuio_sys_state, 0, sizeof(_patch_piuio_sys_state));
  _patch_piuio_state_initialized = false;

  return CNH_RESULT_SUCCESS;
}

static enum cnh_result _patch_piuio_reset(void)
{
  log_info("Resetting PIUIO: %s", _patch_piuio_api.ident());

  _patch_piuio_api.close();

  if (!_patch_piuio_api.open()) {
    log_error("Resetting api piuio %s failed", _patch_piuio_api.ident());
    return CNH_RESULT_OTHER_ERROR;
  }

  // Clear all state on reset
  memset(_patch_piuio_pad_state, 0, sizeof(_patch_piuio_pad_state));
  memset(&_patch_piuio_sys_state, 0, sizeof(_patch_piuio_sys_state));
  _patch_piuio_state_initialized = false;

  return CNH_RESULT_SUCCESS;
}

static enum cnh_result _patch_piuio_control_msg(
    int request_type,
    int request,
    int value,
    int index,
    struct cnh_iobuf *buffer,
    int timeout)
{
  if (_patch_piuio_poll_delay_ms > 0) {
    usleep(_patch_piuio_poll_delay_ms * 1000);
  }

  /**
   * Expected call pattern for a full game state update on a single frame (when
   * done synchronously)
   *
   * CTRL_OUT: light data + sensor 0
   * CTRL_IN: input data of sensor 0
   * CTRL_OUT: light data + sensor 1
   * CTRL_IN: input data of sensor 1
   * CTRL_OUT: light data + sensor 2
   * CTRL_IN: input data of sensor 2
   * CTRL_OUT: light data + sensor 3
   * CTRL_IN: input data of sensor 3
   *
   * Reduce call overhead to pumptools's piuio API for recv and send polling any
   * potential implementation
   */
  if (request_type == PIUIO_DRV_USB_CTRL_TYPE_IN &&
      request == PIUIO_DRV_USB_CTRL_REQUEST) {
    if (buffer->nbytes != PIUIO_DRV_BUFFER_SIZE) {
      log_error("Invalid buffer size for ctrl in: %d", buffer->nbytes);
      return CNH_RESULT_INVALID_PARAMETER;
    }

#ifdef PATCH_PIUIO_CALL_TRACE
    log_debug("Read");
#endif

    // Only read buffered inputs, no need to trigger another update since
    // this is taken of by the cycle start

    _patch_piuio_read_inputs_to_buffer(buffer);

    return CNH_RESULT_SUCCESS;
  } else if (
      request_type == PIUIO_DRV_USB_CTRL_TYPE_OUT &&
      request == PIUIO_DRV_USB_CTRL_REQUEST) {
    if (buffer->nbytes != PIUIO_DRV_BUFFER_SIZE) {
      log_error("Invalid buffer size for ctrl out: %d", buffer->nbytes);
      return CNH_RESULT_INVALID_PARAMETER;
    }

    _patch_piuio_read_outputs_from_buffer(buffer);

    // Sync properly with sensor cycling by application
    // Note: Naturally, the update code below will break if sensors are not
    // cycled as expected
    _patch_piuio_sensor_group =
        _patch_piuio_get_sensor_group_from_buffer(buffer);

#ifdef PATCH_PIUIO_CALL_TRACE
    log_debug("Write: %d", _patch_piuio_sensor_group);
#endif

    // Trigger exactly ONE full update cycle on the API implementation on
    // every first call of the whole update cycle

    if (_patch_piuio_sensor_group == 0) {
#ifdef PATCH_PIUIO_CALL_TRACE
      log_debug("Update API");
#endif

      if (!_patch_piuio_api.send()) {
        log_error(
            "Sending outputs on api piuio %s failed", _patch_piuio_api.ident());
        return CNH_RESULT_OTHER_ERROR;
      }

      if (!_patch_piuio_api.recv()) {
        log_error(
            "Receiving inputs on api piuio %s failed",
            _patch_piuio_api.ident());
        return CNH_RESULT_OTHER_ERROR;
      }
    }

    return CNH_RESULT_SUCCESS;
  } else if (request_type == 0 && request == 0) {
    // ITG 2/PIU Pro kernel hack, can be handled by piuio-khack module
    // Safety net for visibility if the module is missing
    log_error("Unhandled PIUIO kernel hack request detected, cannot dispatch");

    return CNH_RESULT_INVALID_PARAMETER;
  } else {
    log_error("Invalid usb control request to PIUIO: 0x%02X", request);
    return CNH_RESULT_INVALID_PARAMETER;
  }
}

static void _patch_piuio_close(void)
{
  log_info("Closing PIUIO: %s", _patch_piuio_api.ident());

  _patch_piuio_api.close();
}

static void _patch_piuio_read_inputs_to_buffer(struct cnh_iobuf *buffer)
{
  struct ptapi_io_piuio_pad_inputs p1_pad_in;
  struct ptapi_io_piuio_pad_inputs p2_pad_in;
  struct ptapi_io_piuio_sys_inputs sys_in;

  memset(&p1_pad_in, 0, sizeof(p1_pad_in));
  memset(&p2_pad_in, 0, sizeof(p2_pad_in));
  memset(&sys_in, 0, sizeof(sys_in));

  // Read current inputs from API
  _patch_piuio_api.get_input_pad(0, _patch_piuio_sensor_group, &p1_pad_in);
  _patch_piuio_api.get_input_pad(1, _patch_piuio_sensor_group, &p2_pad_in);
  _patch_piuio_api.get_input_sys(&sys_in);

  // Initialize state on first call
  if (!_patch_piuio_state_initialized) {
    _patch_piuio_pad_state[0][_patch_piuio_sensor_group] = p1_pad_in;
    _patch_piuio_pad_state[1][_patch_piuio_sensor_group] = p2_pad_in;
    _patch_piuio_sys_state = sys_in;
    _patch_piuio_state_initialized = true;
  }

  // Initialize buffer with all bits set (PIUIO uses inverted logic)
  memset(buffer->bytes, 0xFF, PIUIO_DRV_BUFFER_SIZE);

  /*
     PIUIO Protocol Format (inverted logic - 0 = pressed, 1 = not pressed):
     
     byte 0: Player 1 pad sensors
     bit 0: sensor p1: LU (left-up)
     bit 1: sensor p1: RU (right-up) 
     bit 2: sensor p1: CN (center)
     bit 3: sensor p1: LD (left-down)
     bit 4: sensor p1: RD (right-down)
     bits 5-7: unused

     byte 1: System controls
     bit 1: test button
     bit 2: coin 1
     bit 6: service button  
     bit 7: clear button
     other bits: unused

     byte 2: Player 2 pad sensors
     bit 0: sensor p2: LU
     bit 1: sensor p2: RU
     bit 2: sensor p2: CN
     bit 3: sensor p2: LD
     bit 4: sensor p2: RD
     bits 5-7: unused

     byte 3: Additional system inputs
     bit 2: coin 2
     other bits: unused

     bytes 4-7: unused
  */

  // Player 1 inputs - clear bits for pressed buttons
  if (p1_pad_in.lu) buffer->bytes[0] &= ~(1 << 0);
  if (p1_pad_in.ru) buffer->bytes[0] &= ~(1 << 1);
  if (p1_pad_in.cn) buffer->bytes[0] &= ~(1 << 2);
  if (p1_pad_in.ld) buffer->bytes[0] &= ~(1 << 3);
  if (p1_pad_in.rd) buffer->bytes[0] &= ~(1 << 4);

  // Player 2 inputs - clear bits for pressed buttons
  if (p2_pad_in.lu) buffer->bytes[2] &= ~(1 << 0);
  if (p2_pad_in.ru) buffer->bytes[2] &= ~(1 << 1);
  if (p2_pad_in.cn) buffer->bytes[2] &= ~(1 << 2);
  if (p2_pad_in.ld) buffer->bytes[2] &= ~(1 << 3);
  if (p2_pad_in.rd) buffer->bytes[2] &= ~(1 << 4);

  // System inputs - clear bits for pressed buttons
  if (sys_in.test) buffer->bytes[1] &= ~(1 << 1);
  if (sys_in.service) buffer->bytes[1] &= ~(1 << 6);
  if (sys_in.clear) buffer->bytes[1] &= ~(1 << 7);
  if (sys_in.coin) buffer->bytes[1] &= ~(1 << 2);

  // Coin 2 - handle carefully to prevent false service menu triggers
  if (sys_in.coin2) {
    buffer->bytes[3] &= ~(1 << 2);
  }
  // Note: Other bits in byte 3 remain set to 1 to prevent false triggers

  // Update state for next cycle
  _patch_piuio_pad_state[0][_patch_piuio_sensor_group] = p1_pad_in;
  _patch_piuio_pad_state[1][_patch_piuio_sensor_group] = p2_pad_in;
  _patch_piuio_sys_state = sys_in;

  buffer->pos = PIUIO_DRV_BUFFER_SIZE;

#ifdef PATCH_PIUIO_CALL_TRACE
  log_debug("Input buffer: %02X %02X %02X %02X %02X %02X %02X %02X", 
            buffer->bytes[0], buffer->bytes[1], buffer->bytes[2], buffer->bytes[3],
            buffer->bytes[4], buffer->bytes[5], buffer->bytes[6], buffer->bytes[7]);
#endif
}

static void _patch_piuio_read_outputs_from_buffer(struct cnh_iobuf *buffer)
{
  struct ptapi_io_piuio_pad_outputs p1_pad_out;
  struct ptapi_io_piuio_pad_outputs p2_pad_out;
  struct ptapi_io_piuio_cab_outputs cab_out;

  /*
   PIUIO Output Format:
   
   byte 0:
   bit 0-1: sensor id (00=right, 01=left, 10=down, 11=up)
   bit 2: pad light p1: LU
   bit 3: pad light p1: RU
   bit 4: pad light p1: CN
   bit 5: pad light p1: LD
   bit 6: pad light p1: RD

   byte 1:
   bit 2: bass/neon lights
   bit 3: coin counter 2 (front USB enable)

   byte 2:
   bit 2: pad light p2: LU
   bit 3: pad light p2: RU
   bit 4: pad light p2: CN
   bit 5: pad light p2: LD
   bit 6: pad light p2: RD
   bit 7: halogen R2

   byte 3:
   bit 0: halogen R1
   bit 1: halogen L2
   bit 2: halogen L1
   bit 4: coin counter 1

   bytes 4-7: unused
  */

  memset(&p1_pad_out, 0, sizeof(p1_pad_out));
  memset(&p2_pad_out, 0, sizeof(p2_pad_out));
  memset(&cab_out, 0, sizeof(cab_out));

  // Player 1 pad lights
  p1_pad_out.lu = (buffer->bytes[0] & (1 << 2)) != 0;
  p1_pad_out.ru = (buffer->bytes[0] & (1 << 3)) != 0;
  p1_pad_out.cn = (buffer->bytes[0] & (1 << 4)) != 0;
  p1_pad_out.ld = (buffer->bytes[0] & (1 << 5)) != 0;
  p1_pad_out.rd = (buffer->bytes[0] & (1 << 6)) != 0;

  // Player 2 pad lights
  p2_pad_out.lu = (buffer->bytes[2] & (1 << 2)) != 0;
  p2_pad_out.ru = (buffer->bytes[2] & (1 << 3)) != 0;
  p2_pad_out.cn = (buffer->bytes[2] & (1 << 4)) != 0;
  p2_pad_out.ld = (buffer->bytes[2] & (1 << 5)) != 0;
  p2_pad_out.rd = (buffer->bytes[2] & (1 << 6)) != 0;

  // Cabinet lights
  cab_out.bass = (buffer->bytes[1] & (1 << 2)) != 0;
  cab_out.halo_r2 = (buffer->bytes[2] & (1 << 7)) != 0;
  cab_out.halo_r1 = (buffer->bytes[3] & (1 << 0)) != 0;
  cab_out.halo_l2 = (buffer->bytes[3] & (1 << 1)) != 0;
  cab_out.halo_l1 = (buffer->bytes[3] & (1 << 2)) != 0;

  // Send outputs to API
  _patch_piuio_api.set_output_pad(0, &p1_pad_out);
  _patch_piuio_api.set_output_pad(1, &p2_pad_out);
  _patch_piuio_api.set_output_cab(&cab_out);

#ifdef PATCH_PIUIO_CALL_TRACE
  log_debug("Output: P1[%d%d%d%d%d] P2[%d%d%d%d%d] Cab[%d%d%d%d%d]",
            p1_pad_out.lu, p1_pad_out.ru, p1_pad_out.cn, p1_pad_out.ld, p1_pad_out.rd,
            p2_pad_out.lu, p2_pad_out.ru, p2_pad_out.cn, p2_pad_out.ld, p2_pad_out.rd,
            cab_out.bass, cab_out.halo_r1, cab_out.halo_r2, cab_out.halo_l1, cab_out.halo_l2);
#endif
}

static enum ptapi_io_piuio_sensor_group
_patch_piuio_get_sensor_group_from_buffer(struct cnh_iobuf *buffer)
{
  return (enum ptapi_io_piuio_sensor_group)(buffer->bytes[0] & 0x03);
}