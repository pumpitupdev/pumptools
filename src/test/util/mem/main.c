#include <cmocka/cmocka.h>

#include "test-util/mem.h"

#include "util/mem.h"

static void test_util_mem_mock_interface(void** state)
{
    void* ptr;

    test_util_mem_install_mem_interface();

    ptr = util_xmalloc(100);
    assert_ptr_not_equal(ptr, NULL);

    ptr = util_xrealloc(ptr, 200);
    assert_ptr_not_equal(ptr, NULL);

    util_xfree(&ptr);
    assert_ptr_equal(ptr, NULL);
}

static void test_util_mem_real(void** state)
{
    void* ptr;

    util_mem_init_default();

    ptr = util_xmalloc(100);
    assert_ptr_not_equal(ptr, NULL);

    ptr = util_xrealloc(ptr, 200);
    assert_ptr_not_equal(ptr, NULL);

    util_xfree(&ptr);
    assert_ptr_equal(ptr, NULL);

    // Should not crash
    util_xfree(&ptr);
}

int main(int argc, char* argv[])
{
    const struct CMUnitTest tests[] ={
        cmocka_unit_test(test_util_mem_mock_interface),
        cmocka_unit_test(test_util_mem_real)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}