#include <errno.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka/cmocka.h>

#include "capnhook/hook/lib.h"

static const char *group_file_content;
static char *group_file_buffer;
static size_t group_file_close_count;
static size_t real_getgrnam_r_call_count;

FILE *__wrap_fopen(const char *path, const char *mode)
{
  assert_string_equal(path, "/etc/group");
  assert_string_equal(mode, "r");

  group_file_buffer = strdup(group_file_content);

  return fmemopen(group_file_buffer, strlen(group_file_buffer), "r");
}

int __real_fclose(FILE *stream);

int __wrap_fclose(FILE *stream)
{
  int ret;

  group_file_close_count++;
  ret = __real_fclose(stream);
  free(group_file_buffer);
  group_file_buffer = NULL;

  return ret;
}

static int getgrnam_r_mock(
    const char *name,
    struct group *grp,
    char *buf,
    size_t buflen,
    struct group **result)
{
  real_getgrnam_r_call_count++;
  *result = NULL;

  return ENOENT;
}

static void free_group_result(struct group *grp)
{
  if (grp->gr_mem != NULL) {
    for (char **member = grp->gr_mem; *member != NULL; member++) {
      free(*member);
    }

    free(grp->gr_mem);
  }

  free(grp->gr_name);
  free(grp->gr_passwd);
}

static int setup(void **state)
{
  struct cnh_lib_unit_test_func_mocks *func_mocks;

  func_mocks = cnh_lib_allocate_func_mocks(1);
  func_mocks[0].name = "getgrnam_r";
  func_mocks[0].func = getgrnam_r_mock;
  cnh_lib_init_unit_test(func_mocks, 1);

  group_file_content = "";
  group_file_buffer = NULL;
  group_file_close_count = 0;
  real_getgrnam_r_call_count = 0;

  return 0;
}

static int teardown(void **state)
{
  free(group_file_buffer);
  group_file_buffer = NULL;
  cnh_lib_shutdown_unit_test();

  return 0;
}

static void assert_audio_group(
    const char *content,
    gid_t expected_gid,
    const char *const *expected_members,
    size_t expected_member_count)
{
  struct group grp = {0};
  struct group *result = NULL;
  char buf[256];
  int ret;

  group_file_content = content;
  ret = getgrnam_r("audio", &grp, buf, sizeof(buf), &result);

  assert_int_equal(ret, 0);
  assert_ptr_equal(result, &grp);
  assert_string_equal(grp.gr_name, "audio");
  assert_string_equal(grp.gr_passwd, "x");
  assert_int_equal(grp.gr_gid, expected_gid);

  for (size_t i = 0; i < expected_member_count; i++) {
    assert_non_null(grp.gr_mem[i]);
    assert_string_equal(grp.gr_mem[i], expected_members[i]);
  }

  assert_null(grp.gr_mem[expected_member_count]);
  assert_int_equal(group_file_close_count, 1);

  free_group_result(&grp);
}

static void assert_audio_group_rejected(const char *content)
{
  struct group grp = {0};
  struct group *result = NULL;
  char buf[256];
  int ret;

  group_file_content = content;
  ret = getgrnam_r("audio", &grp, buf, sizeof(buf), &result);

  assert_int_not_equal(ret, 0);
  assert_null(result);
  assert_int_equal(group_file_close_count, 1);

  free_group_result(&grp);
}

static void test_exact_audio_group_after_name_decoy(void **state)
{
  const char *members[] = {"alice", "bob"};

  assert_audio_group(
      "notaudio:x:1000:someone\n"
      "audio:x:29:alice,bob\n",
      29,
      members,
      2);
}

static void test_exact_audio_group_after_member_decoy(void **state)
{
  const char *members[] = {"alice"};

  assert_audio_group(
      "staff:x:50:audioadmin\n"
      "audio:x:29:alice\n",
      29,
      members,
      1);
}

static void test_audio_group_without_final_newline(void **state)
{
  const char *members[] = {"alice", "bob", "carol"};

  assert_audio_group("audio:x:29:alice,bob,carol", 29, members, 3);
}

static void test_audio_group_after_garbage_lines(void **state)
{
  const char *members[] = {"alice"};

  assert_audio_group(
      "garbage\n"
      ":not:a:group:\n"
      "audio:x:29:alice\n"
      "more garbage\n",
      29,
      members,
      1);
}

static void test_missing_audio_group(void **state)
{
  assert_audio_group_rejected(
      "root:x:0:\n"
      "video:x:44:alice\n");
}

static void test_group_with_extra_field(void **state)
{
  assert_audio_group_rejected("audio:x:29:alice:garbage\n");
}

static void test_group_with_non_numeric_gid(void **state)
{
  assert_audio_group_rejected("audio:x:not-a-number:alice\n");
}

static void test_group_with_trailing_gid_garbage(void **state)
{
  assert_audio_group_rejected("audio:x:29garbage:alice\n");
}

static void test_group_with_missing_fields(void **state)
{
  assert_audio_group_rejected("audio:x:29\n");
}

static void test_only_garbage(void **state)
{
  assert_audio_group_rejected("garbage\n\001\002\003\nnot a group\n");
}

static void test_unrelated_lookup_delegates(void **state)
{
  struct group grp = {0};
  struct group *result = NULL;
  char buf[64];
  int ret;

  ret = getgrnam_r("video", &grp, buf, sizeof(buf), &result);

  assert_int_equal(ret, ENOENT);
  assert_null(result);
  assert_int_equal(real_getgrnam_r_call_count, 1);
  assert_int_equal(group_file_close_count, 0);
}

int main(int argc, char *argv[])
{
  const struct CMUnitTest tests[] = {
      cmocka_unit_test_setup_teardown(
          test_exact_audio_group_after_name_decoy, setup, teardown),
      cmocka_unit_test_setup_teardown(
          test_exact_audio_group_after_member_decoy, setup, teardown),
      cmocka_unit_test_setup_teardown(
          test_audio_group_without_final_newline, setup, teardown),
      cmocka_unit_test_setup_teardown(
          test_audio_group_after_garbage_lines, setup, teardown),
      cmocka_unit_test_setup_teardown(
          test_missing_audio_group, setup, teardown),
      cmocka_unit_test_setup_teardown(
          test_group_with_extra_field, setup, teardown),
      cmocka_unit_test_setup_teardown(
          test_group_with_non_numeric_gid, setup, teardown),
      cmocka_unit_test_setup_teardown(
          test_group_with_trailing_gid_garbage, setup, teardown),
      cmocka_unit_test_setup_teardown(
          test_group_with_missing_fields, setup, teardown),
      cmocka_unit_test_setup_teardown(test_only_garbage, setup, teardown),
      cmocka_unit_test_setup_teardown(
          test_unrelated_lookup_delegates, setup, teardown)};

  return cmocka_run_group_tests(tests, NULL, NULL);
}
