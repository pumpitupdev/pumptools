#include <stdlib.h>

#include <cmocka/cmocka.h>

#include "capnhook/hook/lib.h"

#include "hook/propatch/usb-fix.h"

#include "test-util/mem.h"

#include "util/log.h"
#include "util/mem.h"

/*
 * Existing 12-token sysfs layout. The USB identifier is token 5 ("2-2"),
 * which is the fixed position expected by the current parser:
 *
 *   ../devices/.../0000:00:14.0/usb2/2-2/.../block/sde
 *                                      ^ token 5
 */
static const char *readlink_return_value_12_tokens =
    "../devices/pci0000:00/0000:00:14.0/usb2/2-2/2-2:1.0/host7/target7:0:0/"
    "7:0:0:0/block/sde";

/*
 * A second PCI device in the topology adds one path token. The USB identifier
 * is consequently token 6 instead of token 5:
 *
 *   ../devices/.../0000:00:1c.0/0000:03:00.0/usb2/2-2/.../block/sde
 *                                                   ^ token 6
 *
 * PR #68 is intended to recognize this 13-token form and find "2-2" without
 * assuming a fixed token position.
 */
static const char *readlink_return_value_13_tokens =
    "../devices/pci0000:00/0000:00:1c.0/0000:03:00.0/usb2/2-2/2-2:1.0/"
    "host7/target7:0:0/7:0:0:0/block/sde";
static const char *readlink_return_value_13_tokens_unconfigured_port =
    "../devices/pci0000:00/0000:00:1c.0/0000:03:00.0/usb2/3-4/3-4:1.0/"
    "host7/target7:0:0/7:0:0:0/block/sde";
static const char *readlink_return_value_13_tokens_no_usb_id =
    "../devices/pci0000:00/0000:00:1c.0/0000:03:00.0/usb2/device/"
    "device:1.0/host7/target7:0:0/7:0:0:0/block/sde";
static const char *readlink_return_value_13_tokens_bad_suffix =
    "../devices/pci0000:00/0000:00:1c.0/0000:03:00.0/usb2/2-2/2-2:1.0/"
    "host7/target7:0:0/7:0:0:0/not-block/sde";

static const char *readlink_return_value;
static const char *player1_match_buf =
    "../devices/pci0000:00/0000:00:14.0/usb2/1-1/1-1:1.0";
static const char *player1_13_tokens_match_buf =
    "../devices/pci0000:00/0000:00:1c.0/0000:03:00.0/1-2/1-2:1.0";
static const char *player2_match_buf =
    "../devices/pci0000:00/0000:00:14.0/usb2/1-2/1-2:1.0";

extern ssize_t
propatch_usb_fix_readlink(const char *path, char *buf, size_t len);

static uint32_t readlink_call_count = 0;

ssize_t readlink_mock(const char *path, char *buf, size_t len)
{
  ssize_t len_return;

  readlink_call_count++;

  len_return = strlen(readlink_return_value);

  assert_true(len >= len_return);

  strcpy(buf, readlink_return_value);

  return len_return;
}

static int setup(void **state)
{
  struct cnh_lib_unit_test_func_mocks *func_mocks;
  size_t func_mocks_cnt;

  test_util_mem_install_mem_interface();

  func_mocks_cnt = 1;
  func_mocks = cnh_lib_allocate_func_mocks(func_mocks_cnt);

  func_mocks[0].name = "readlink";
  func_mocks[0].func = readlink_mock;

  cnh_lib_init_unit_test(func_mocks, func_mocks_cnt);

  readlink_return_value = readlink_return_value_12_tokens;
  readlink_call_count = 0;

  return 0;
}

static int teardown(void **state)
{
  propatch_usb_fix_shutdown();

  cnh_lib_shutdown_unit_test();

  return 0;
}

static void test_usb_fix_disabled(void **state)
{
  ssize_t res;
  char buffer[256];

  res = propatch_usb_fix_readlink(
      "/sys/block/sde/device", buffer, sizeof(buffer));

  assert_int_equal(res, strlen(readlink_return_value));
  assert_string_equal(buffer, readlink_return_value);

  assert_int_equal(readlink_call_count, 1);
}

static void test_no_match_usb_fix(void **state)
{
  ssize_t res;
  char buffer[256];

  propatch_usb_fix_init("sdd,sdf", "2-1", "2-2");

  res = propatch_usb_fix_readlink(
      "/sys/block/sde/device", buffer, sizeof(buffer));

  assert_int_equal(res, strlen(readlink_return_value));

  assert_int_equal(readlink_call_count, 1);
}

static void test_match_player1_usb_fix(void **state)
{
  ssize_t res;
  char buffer[256];

  propatch_usb_fix_init("sde,sdf", "2-1", "2-2");

  res = propatch_usb_fix_readlink(
      "/sys/block/sde/device", buffer, sizeof(buffer));

  assert_int_equal(res, strlen(player1_match_buf));
  assert_string_equal(buffer, player1_match_buf);

  assert_int_equal(readlink_call_count, 1);
}

static void test_match_player2_usb_fix(void **state)
{
  ssize_t res;
  char buffer[256];

  propatch_usb_fix_init("sde,sdf", "2-2", "2-1");

  res = propatch_usb_fix_readlink(
      "/sys/block/sde/device", buffer, sizeof(buffer));

  assert_int_equal(res, strlen(player2_match_buf));
  assert_string_equal(buffer, player2_match_buf);

  assert_int_equal(readlink_call_count, 1);
}

static void test_match_player1_usb_fix_13_tokens(void **state)
{
  ssize_t res;
  char buffer[256];

  /*
   * The assigned sde drive is on configured bus-port 2-2. Its extra PCI path
   * component must not prevent the link from being rewritten for player 1.
   * This is the regression fixed by PR #68 and intentionally fails before it.
   */
  readlink_return_value = readlink_return_value_13_tokens;
  propatch_usb_fix_init("sde,sdf", "2-2", "2-1");

  res = propatch_usb_fix_readlink(
      "/sys/block/sde/device", buffer, sizeof(buffer));

  assert_int_equal(res, strlen(player1_13_tokens_match_buf));
  assert_string_equal(buffer, player1_13_tokens_match_buf);

  assert_int_equal(readlink_call_count, 1);
}

static void test_unconfigured_port_usb_fix_13_tokens(void **state)
{
  ssize_t res;
  char buffer[256];

  /*
   * A valid 13-token link on bus-port 3-4 is neither configured player port.
   * It must pass through unchanged rather than being rewritten or rejected as
   * malformed. This also intentionally fails before the PR #68 parser change.
   */
  readlink_return_value = readlink_return_value_13_tokens_unconfigured_port;
  propatch_usb_fix_init("sde,sdf", "2-2", "2-1");

  res = propatch_usb_fix_readlink(
      "/sys/block/sde/device", buffer, sizeof(buffer));

  assert_int_equal(res, strlen(readlink_return_value));
  assert_string_equal(buffer, readlink_return_value);

  assert_int_equal(readlink_call_count, 1);
}

static void test_unassigned_device_usb_fix_13_tokens(void **state)
{
  ssize_t res;
  char buffer[256];

  /* Only configured device nodes enter path parsing, regardless of depth. */
  readlink_return_value = readlink_return_value_13_tokens;
  propatch_usb_fix_init("sdd,sdf", "2-2", "2-1");

  res = propatch_usb_fix_readlink(
      "/sys/block/sde/device", buffer, sizeof(buffer));

  assert_int_equal(res, strlen(readlink_return_value));
  assert_string_equal(buffer, readlink_return_value);

  assert_int_equal(readlink_call_count, 1);
}

static void test_missing_usb_id_usb_fix_13_tokens(void **state)
{
  ssize_t res;
  char buffer[256];

  /* A matching device without a bus-port token cannot be mapped safely. */
  readlink_return_value = readlink_return_value_13_tokens_no_usb_id;
  propatch_usb_fix_init("sde,sdf", "2-2", "2-1");

  res = propatch_usb_fix_readlink(
      "/sys/block/sde/device", buffer, sizeof(buffer));

  assert_int_equal(res, 0);
  assert_int_equal(readlink_call_count, 1);
}

static void test_bad_suffix_usb_fix_13_tokens(void **state)
{
  ssize_t res;
  char buffer[256];

  /* The final two components must remain block/<device>. */
  readlink_return_value = readlink_return_value_13_tokens_bad_suffix;
  propatch_usb_fix_init("sde,sdf", "2-2", "2-1");

  res = propatch_usb_fix_readlink(
      "/sys/block/sde/device", buffer, sizeof(buffer));

  assert_int_equal(res, 0);
  assert_int_equal(readlink_call_count, 1);
}

int main(int argc, char *argv[])
{
  const struct CMUnitTest tests[] = {
      cmocka_unit_test_setup_teardown(test_usb_fix_disabled, setup, teardown),
      cmocka_unit_test_setup_teardown(test_no_match_usb_fix, setup, teardown),
      cmocka_unit_test_setup_teardown(
          test_match_player1_usb_fix, setup, teardown),
      cmocka_unit_test_setup_teardown(
          test_match_player2_usb_fix, setup, teardown),
      cmocka_unit_test_setup_teardown(
          test_match_player1_usb_fix_13_tokens, setup, teardown),
      cmocka_unit_test_setup_teardown(
          test_unconfigured_port_usb_fix_13_tokens, setup, teardown),
      cmocka_unit_test_setup_teardown(
          test_unassigned_device_usb_fix_13_tokens, setup, teardown),
      cmocka_unit_test_setup_teardown(
          test_missing_usb_id_usb_fix_13_tokens, setup, teardown),
      cmocka_unit_test_setup_teardown(
          test_bad_suffix_usb_fix_13_tokens, setup, teardown)};

  return cmocka_run_group_tests(tests, NULL, NULL);
}
