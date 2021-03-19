#define LOG_MODULE "util-test"

#include <string.h>

#include "list.h"
#include "log.h"
#include "mem.h"
#include "str.h"
#include "test.h"

struct util_test_case {
  struct util_list_node head;
  char *name;
  util_test_func_t func;
};

static size_t util_test_count;
static struct util_list util_test_list;

void util_test_init()
{
  util_test_count = 0;
  util_list_init(&util_test_list);

  log_info("Initialized");
}

void util_test_add(const char *name, util_test_func_t func)
{
  struct util_test_case *test;

  test = util_xmalloc(sizeof(struct util_test_case));

  test->name = util_str_dup(name);
  test->func = func;

  util_list_append(&util_test_list, &test->head);
  util_test_count++;

  log_info("Added %s, func %p", name, func);
}

void util_test_exec()
{
  log_info("Exec %d tests", util_test_count);

  struct util_list_node *pos;
  struct util_test_case *test;
  size_t counter = 1;

  for (pos = util_test_list.head; pos != NULL; pos = pos->next) {
    test = containerof(pos, struct util_test_case, head);

    log_debug("%d/%d Executing %s...", counter++, util_test_count, test->name);

    if (!test->func()) {
      log_error("%s failed", test->name);
      return;
    }
  }

  log_info("All tests passed");
}

void util_test_shutdown()
{
  log_info("Shutdown");

  struct util_list_node *pos;
  struct util_test_case *test;

  // TODO list elems cleanup?

  for (pos = util_test_list.head; pos != NULL; pos = pos->next) {
    test = containerof(pos, struct util_test_case, head);

    free(test->name);
  }
}

bool util_test_assert_exp(
    bool exp, const char *exp_str, const char *file, size_t line_num)
{
  if (!exp) {
    log_error(
        "Assert failed [%s:%d]: Expression %s failed", file, line_num, exp_str);
    return false;
  } else {
    log_debug(
        "Assert successful [%s:%d]: Expression %s", file, line_num, exp_str);
    return true;
  }
}

bool util_test_assert_uint(
    size_t val_exp,
    size_t val_actual,
    const char *exp,
    const char *actual,
    const char *file,
    size_t line_num)
{
  if (val_exp != val_actual) {
    log_error(
        "Assert failed [%s:%d]: %d != %d of expected %s and actual %s",
        file,
        line_num,
        val_exp,
        val_actual,
        exp,
        actual);
    return false;
  } else {
    log_debug(
        "Assert successful [%s:%d]: %d == %d of expected %s and actual %s",
        file,
        line_num,
        val_exp,
        val_actual,
        exp,
        actual);
    return true;
  }
}

bool util_test_assert_str(
    const char *val_exp,
    const char *val_actual,
    const char *exp,
    const char *actual,
    const char *file,
    size_t line_num)
{
  if (strcmp(val_exp, val_actual) != 0) {
    log_error(
        "Assert failed [%s:%d]: %s != %s of expected %s and actual %s",
        file,
        line_num,
        val_exp,
        val_actual,
        exp,
        actual);
    return false;
  } else {
    log_debug(
        "Assert successful [%s:%d]: %s == %s of expected %s and actual %s",
        file,
        line_num,
        val_exp,
        val_actual,
        exp,
        actual);
    return true;
  }
}

bool util_test_assert_data(
    const void *data_exp,
    const void *data_actual,
    size_t len,
    const char *exp,
    const char *actual,
    const char *file,
    size_t line_num)
{
  bool ret;
  char *data_exp_str = util_str_buffer(data_exp, len);
  char *data_actual_str = util_str_buffer(data_actual, len);

  if (memcmp(data_exp, data_actual, len) != 0) {
    log_error(
        "Assert failed [%s:%d]: %s != %s of expected %s and actual %s",
        file,
        line_num,
        data_exp_str,
        data_actual_str,
        exp,
        actual);

    uint32_t cnt = 0;

    for (size_t i = 0; i < len; i++) {
      /* print the first 5 diffs, only */
      if (((const uint8_t *) data_exp)[i] !=
          ((const uint8_t *) data_actual)[i]) {
        if (cnt < 5) {
          log_error(
              "pos %d: %x != %x",
              i,
              ((const uint8_t *) data_exp)[i],
              ((const uint8_t *) data_actual)[i]);
        }

        cnt++;
      }
    }

    if (cnt > 5) {
      log_error("...and more different bytes, total diffs %d", cnt);
    }

    ret = false;
  } else {
    log_debug(
        "Assert successful [%s:%d]: %s == %s of expected %s and actual %s",
        file,
        line_num,
        data_exp_str,
        data_actual_str,
        exp,
        actual);
    ret = true;
  }

  free(data_exp_str);
  free(data_actual_str);

  return ret;
}