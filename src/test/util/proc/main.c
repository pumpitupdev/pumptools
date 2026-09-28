#include <cmocka/cmocka.h>
#include <dlfcn.h>
#include <linux/limits.h>
#include <string.h>

#include "util/proc.h"

static void test_get_folder_path_shared_object(void **state)
{
  char path[PATH_MAX];
  char expected[PATH_MAX];
  Dl_info info;
  char *separator;

  assert_true(util_proc_get_folder_path_shared_object(
      (void *) test_get_folder_path_shared_object, path, sizeof(path)));

  assert_true(dladdr((void *) test_get_folder_path_shared_object, &info));
  assert_non_null(info.dli_fname);
  assert_true(strlen(info.dli_fname) < sizeof(expected));
  strcpy(expected, info.dli_fname);
  separator = strrchr(expected, '/');
  assert_non_null(separator);

  if (separator == expected) {
    separator[1] = '\0';
  } else {
    *separator = '\0';
  }

  assert_string_equal(path, expected);
}

static void test_get_folder_path_shared_object_invalid_args(void **state)
{
  char path[PATH_MAX];

  assert_false(
      util_proc_get_folder_path_shared_object(NULL, path, sizeof(path)));
  assert_false(util_proc_get_folder_path_shared_object(
      (void *) test_get_folder_path_shared_object_invalid_args,
      NULL,
      sizeof(path)));
  assert_false(util_proc_get_folder_path_shared_object(
      (void *) test_get_folder_path_shared_object_invalid_args, path, 0));
}

static void test_get_folder_path_shared_object_small_buffer(void **state)
{
  char path[1];

  assert_false(util_proc_get_folder_path_shared_object(
      (void *) test_get_folder_path_shared_object_small_buffer,
      path,
      sizeof(path)));
}

int main(int argc, char *argv[])
{
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_get_folder_path_shared_object),
      cmocka_unit_test(test_get_folder_path_shared_object_invalid_args),
      cmocka_unit_test(test_get_folder_path_shared_object_small_buffer)};

  return cmocka_run_group_tests(tests, NULL, NULL);
}
