#include <stdlib.h>
#include <string.h>

#include <cmocka/cmocka.h>

#include "capnhook/hooklib/usb-emu.h"

#include "hook/patch/piuio.h"

#include "io/piuio/defs.h"

#include "ptapi/io/piuio.h"

static const struct cnh_usb_emu_virtdev_ep *virtdev;
static struct ptapi_io_piuio_pad_inputs pad_inputs[2];
static struct ptapi_io_piuio_sys_inputs sys_inputs;
static struct ptapi_io_piuio_pad_outputs pad_outputs[2];
static struct ptapi_io_piuio_cab_outputs cab_outputs;
static enum ptapi_io_piuio_sensor_group requested_sensor_group;
static unsigned int send_count;
static unsigned int recv_count;

static const char *piuio_ident(void)
{
  return "test";
}

static bool piuio_open(void)
{
  return true;
}

static void piuio_close(void)
{
}

static bool piuio_recv(void)
{
  recv_count++;
  return true;
}

static bool piuio_send(void)
{
  send_count++;
  return true;
}

static void piuio_get_input_pad(
    uint8_t player,
    enum ptapi_io_piuio_sensor_group sensor_group,
    struct ptapi_io_piuio_pad_inputs *inputs)
{
  requested_sensor_group = sensor_group;
  *inputs = pad_inputs[player];
}

static void piuio_get_input_sys(struct ptapi_io_piuio_sys_inputs *inputs)
{
  *inputs = sys_inputs;
}

static void
piuio_set_output_pad(uint8_t player, struct ptapi_io_piuio_pad_outputs *outputs)
{
  pad_outputs[player] = *outputs;
}

static void
piuio_set_output_cab(const struct ptapi_io_piuio_cab_outputs *outputs)
{
  cab_outputs = *outputs;
}

bool ptapi_io_piuio_util_lib_load(
    const char *path, struct ptapi_io_piuio_api *api)
{
  api->ident = piuio_ident;
  api->open = piuio_open;
  api->close = piuio_close;
  api->recv = piuio_recv;
  api->send = piuio_send;
  api->get_input_pad = piuio_get_input_pad;
  api->get_input_sys = piuio_get_input_sys;
  api->set_output_pad = piuio_set_output_pad;
  api->set_output_cab = piuio_set_output_cab;

  return true;
}

void cnh_usb_emu_add_virtdevep(const struct cnh_usb_emu_virtdev_ep *endpoint)
{
  virtdev = endpoint;
}

static int setup(void **state)
{
  memset(pad_inputs, 0, sizeof(pad_inputs));
  memset(&sys_inputs, 0, sizeof(sys_inputs));
  memset(pad_outputs, 0, sizeof(pad_outputs));
  memset(&cab_outputs, 0, sizeof(cab_outputs));
  requested_sensor_group = PTAPI_IO_PIUIO_SENSOR_GROUP_RIGHT;
  send_count = 0;
  recv_count = 0;

  patch_piuio_init("test", 0);

  assert_non_null(virtdev);
  return 0;
}

static enum cnh_result
control_msg(int request_type, uint8_t *bytes, size_t size)
{
  struct cnh_iobuf buffer = {
      .bytes = bytes,
      .nbytes = size,
      .pos = 0,
  };

  enum cnh_result result = virtdev->control_msg(
      request_type, PIUIO_DRV_USB_CTRL_REQUEST, 0, 0, &buffer, 0);

  if (result == CNH_RESULT_SUCCESS &&
      request_type == PIUIO_DRV_USB_CTRL_TYPE_IN) {
    assert_int_equal(buffer.pos, PIUIO_DRV_BUFFER_SIZE);
  }

  return result;
}

static void test_idle_input_replaces_entire_buffer(void **state)
{
  uint8_t bytes[PIUIO_DRV_BUFFER_SIZE];

  memset(bytes, 0x5A, sizeof(bytes));

  assert_int_equal(
      control_msg(PIUIO_DRV_USB_CTRL_TYPE_IN, bytes, sizeof(bytes)),
      CNH_RESULT_SUCCESS);

  for (size_t i = 0; i < sizeof(bytes); i++) {
    assert_int_equal(bytes[i], 0xFF);
  }
}

static void test_input_mapping(void **state)
{
  uint8_t bytes[PIUIO_DRV_BUFFER_SIZE];
  const uint8_t expected[PIUIO_DRV_BUFFER_SIZE] = {
      0xEA, 0x39, 0xF5, 0xFB, 0xFF, 0xFF, 0xFF, 0xFF};
  const uint8_t complementary_expected[PIUIO_DRV_BUFFER_SIZE] = {
      0xF5, 0xFF, 0xEA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

  pad_inputs[0].lu = true;
  pad_inputs[0].cn = true;
  pad_inputs[0].rd = true;
  pad_inputs[1].ru = true;
  pad_inputs[1].ld = true;
  sys_inputs.test = true;
  sys_inputs.service = true;
  sys_inputs.clear = true;
  sys_inputs.coin = true;
  sys_inputs.coin2 = true;

  memset(bytes, 0, sizeof(bytes));

  assert_int_equal(
      control_msg(PIUIO_DRV_USB_CTRL_TYPE_IN, bytes, sizeof(bytes)),
      CNH_RESULT_SUCCESS);
  assert_memory_equal(bytes, expected, sizeof(expected));

  memset(pad_inputs, 0, sizeof(pad_inputs));
  memset(&sys_inputs, 0, sizeof(sys_inputs));
  pad_inputs[0].ru = true;
  pad_inputs[0].ld = true;
  pad_inputs[1].lu = true;
  pad_inputs[1].cn = true;
  pad_inputs[1].rd = true;

  assert_int_equal(
      control_msg(PIUIO_DRV_USB_CTRL_TYPE_IN, bytes, sizeof(bytes)),
      CNH_RESULT_SUCCESS);
  assert_memory_equal(
      bytes, complementary_expected, sizeof(complementary_expected));
}

static void test_output_mapping_and_sensor_selection(void **state)
{
  uint8_t bytes[PIUIO_DRV_BUFFER_SIZE] = {
      0x57, 0x04, 0xAC, 0x05, 0x00, 0x00, 0x00, 0x00};
  uint8_t input_bytes[PIUIO_DRV_BUFFER_SIZE];

  assert_int_equal(
      control_msg(PIUIO_DRV_USB_CTRL_TYPE_OUT, bytes, sizeof(bytes)),
      CNH_RESULT_SUCCESS);

  assert_true(pad_outputs[0].lu);
  assert_false(pad_outputs[0].ru);
  assert_true(pad_outputs[0].cn);
  assert_false(pad_outputs[0].ld);
  assert_true(pad_outputs[0].rd);
  assert_true(pad_outputs[1].lu);
  assert_true(pad_outputs[1].ru);
  assert_false(pad_outputs[1].cn);
  assert_true(pad_outputs[1].ld);
  assert_false(pad_outputs[1].rd);
  assert_true(cab_outputs.bass);
  assert_true(cab_outputs.halo_r2);
  assert_true(cab_outputs.halo_r1);
  assert_false(cab_outputs.halo_l2);
  assert_true(cab_outputs.halo_l1);
  assert_int_equal(send_count, 0);
  assert_int_equal(recv_count, 0);

  assert_int_equal(
      control_msg(PIUIO_DRV_USB_CTRL_TYPE_IN, input_bytes, sizeof(input_bytes)),
      CNH_RESULT_SUCCESS);
  assert_int_equal(requested_sensor_group, PTAPI_IO_PIUIO_SENSOR_GROUP_UP);

  bytes[0] &= ~0x03;
  assert_int_equal(
      control_msg(PIUIO_DRV_USB_CTRL_TYPE_OUT, bytes, sizeof(bytes)),
      CNH_RESULT_SUCCESS);
  assert_int_equal(send_count, 1);
  assert_int_equal(recv_count, 1);
}

static void test_invalid_buffer_size(void **state)
{
  uint8_t bytes[PIUIO_DRV_BUFFER_SIZE] = {0};

  assert_int_equal(
      control_msg(PIUIO_DRV_USB_CTRL_TYPE_IN, bytes, sizeof(bytes) - 1),
      CNH_RESULT_INVALID_PARAMETER);
  assert_int_equal(
      control_msg(PIUIO_DRV_USB_CTRL_TYPE_OUT, bytes, sizeof(bytes) - 1),
      CNH_RESULT_INVALID_PARAMETER);
}

int main(int argc, char *argv[])
{
  const struct CMUnitTest tests[] = {
      cmocka_unit_test_setup(test_idle_input_replaces_entire_buffer, setup),
      cmocka_unit_test_setup(test_input_mapping, setup),
      cmocka_unit_test_setup(test_output_mapping_and_sensor_selection, setup),
      cmocka_unit_test_setup(test_invalid_buffer_size, setup),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
