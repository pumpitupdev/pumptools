#include <cmocka/cmocka.h>

#include "capnhook/hook/lib.h"

#include "util/mem.h"

typedef int (*test_mock_function_t)(int a);

static int test_mock_function(int a)
{
    return a * 2;
}

static void test_cnh_lib_mock_init_unit_test(void** state)
{
    struct cnh_lib_unit_test_func_mocks* mocks;
    size_t mocks_cnt;
    void* ptr;

    mocks_cnt = 1;
    mocks = cnh_lib_allocate_func_mocks(mocks_cnt);

    mocks[0].name = "test_mock_function";
    mocks[0].func = test_mock_function;

    cnh_lib_init_unit_test(mocks, mocks_cnt);

    ptr = cnh_lib_load("asdf");
    assert_ptr_not_equal(ptr, NULL);

    ptr = cnh_lib_get_func_addr_handle(ptr, "test_mock_function");
    assert_ptr_equal(ptr, test_mock_function);

    assert_int_equal(((test_mock_function_t) ptr)(2), 4);

    ptr = cnh_lib_get_func_addr_handle(ptr, "asdf");
    assert_ptr_equal(ptr, NULL);

    cnh_lib_unload(ptr);
    // Nothing happens here

    ptr = cnh_lib_get_func_addr( "test_mock_function");
    assert_ptr_equal(ptr, test_mock_function);

    cnh_lib_shutdown_unit_test();
}

static void test_cnh_lib_real(void** state)
{
    void* ptr;

    cnh_lib_init();

    ptr = cnh_lib_load("libc.so.6");

    assert_ptr_not_equal(ptr, NULL);
    assert_ptr_equal(cnh_lib_load("asdfasdas"), NULL);

    assert_ptr_not_equal(cnh_lib_get_func_addr_handle(ptr, "printf"), NULL);
    assert_ptr_not_equal(cnh_lib_get_func_addr("printf"), NULL);

    assert_ptr_equal(cnh_lib_get_func_addr("asdasdas"), NULL);
}

int main(int argc, char* argv[])
{
    const struct CMUnitTest tests[] ={
        cmocka_unit_test(test_cnh_lib_mock_init_unit_test),
        cmocka_unit_test(test_cnh_lib_real)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}