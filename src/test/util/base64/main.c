#include <cmocka/cmocka.h>
#include <string.h>

#include "test-util/mem.h"

#include "util/base64.h"
#include "util/mem.h"

static int setup(void** state)
{
    test_util_mem_install_mem_interface();

    return 0;
}

static void test_base64_encode(void** state)
{
    const char* exp_str = "dGhpcyBpcyBhIHRlc3Qgc3RyaW5nIHdpdGggc3R1ZmYgKiYjJDEyMzIxOiJ9e3wrXygp";

    const char* str = "this is a test string with stuff *&#$12321:\"}{|+_()";
    size_t len = strlen(str);

    size_t out_len = 0;

    uint8_t* out_data = util_base64_encode((const uint8_t*) str, len, &out_len);

    assert_int_equal(out_len, strlen(exp_str));
    assert_string_equal(out_data, exp_str);

    util_xfree((void**) &out_data);
}

static void test_base64_decode(void** state)
{
    const char* exp_str = "this is a test string with stuff *&#$12321:\"}{|+_()";

    const char* str = "dGhpcyBpcyBhIHRlc3Qgc3RyaW5nIHdpdGggc3R1ZmYgKiYjJDEyMzIxOiJ9e3wrXygp";
    size_t len = strlen(str);

    size_t out_len = 0;

    uint8_t* out_data = util_base64_decode((const uint8_t*) str, len, &out_len);

    assert_int_equal(out_len, strlen(exp_str));
    assert_string_equal(out_data, exp_str);

    util_xfree((void**) &out_data);
}

int main(int argc, char* argv[])
{
    const struct CMUnitTest tests[] ={
        cmocka_unit_test_setup(test_base64_encode, setup),
        cmocka_unit_test_setup(test_base64_decode, setup),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}