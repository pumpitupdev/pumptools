#include <cmocka/cmocka.h>

#include "test-util/mem.h"

#include "util/mem.h"
#include "util/str.h"

static int setup(void** state)
{
    test_util_mem_install_mem_interface();

    return 0;
}

static void test_str_dup(void** state)
{
    char* str;

    str = util_str_dup("aasdf");
    assert_string_equal(str, "aasdf");

    util_xfree((void**) &str);
}

static void test_str_dup_empty(void** state)
{
    char* str;

    str = util_str_dup("");
    assert_string_equal(str, "");

    util_xfree((void**) &str);
}

static void test_str_split0(void** state)
{
    size_t count;
    char** toks;

    toks = util_str_split("", ",", &count);

    assert_int_equal(count, 0);

    util_str_free_split(toks, count);
}

static void test_str_split1(void** state)
{
    size_t count;
    char** toks;

    toks = util_str_split("asd", ",", &count);

    assert_int_equal(count, 1);
    assert_string_equal(toks[0], "asd");

    util_str_free_split(toks, count);
}

static void test_str_split2(void** state)
{
    size_t count;
    char** toks;

    toks = util_str_split("asd,123", ",", &count);

    assert_int_equal(count, 2);
    assert_string_equal(toks[0], "asd");
    assert_string_equal(toks[1], "123");

    util_str_free_split(toks, count);
}

static void test_str_split3(void** state)
{
    size_t count;
    char** toks;

    toks = util_str_split("/asd/123/ddd/", "/", &count);

    assert_int_equal(count, 3);
    assert_string_equal(toks[0], "asd");
    assert_string_equal(toks[1], "123");
    assert_string_equal(toks[2], "ddd");

    util_str_free_split(toks, count);
}

int main(int argc, char* argv[])
{
    const struct CMUnitTest tests[] ={
        cmocka_unit_test_setup(test_str_dup, setup),
        cmocka_unit_test_setup(test_str_dup_empty, setup),
        cmocka_unit_test_setup(test_str_split0, setup),
        cmocka_unit_test_setup(test_str_split1, setup),
        cmocka_unit_test_setup(test_str_split2, setup),
        cmocka_unit_test_setup(test_str_split3, setup)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}