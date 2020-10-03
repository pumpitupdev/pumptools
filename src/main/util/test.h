#ifndef UTIL_TEST_H
#define UTIL_TEST_H

#include <stdbool.h>
#include <stdlib.h>

#include "defs.h"
#include "log.h"

#define UTIL_TEST_MAIN_START \
    int main() \
    { \
        log_info("Unit test %s", FILENAME); \
        util_test_init(); \

#define UTIL_TEST_MAIN_END \
        util_test_exec(); \
        util_test_shutdown(); \
        return 0; \
    } \

#define UTIL_TEST_FUNC_START(name) \
    bool name() \
{ \

#define UTIL_TEST_FUNC_END \
    return true; \
} \

#define UTIL_TEST_ADD(func) util_test_add(STRINGIFY(func), func);

#define UTIL_TEST_ASSERT(exp) \
    do { \
        if (!util_test_assert_exp(exp, STRINGIFY(exp), FILENAME, __LINE__)) { \
            return false; \
        } \
    } while (0); \

#define UTIL_TEST_ASSERT_UINT(exp, actual) \
    do { \
        if (!util_test_assert_uint(exp, actual, STRINGIFY(exp), STRINGIFY(actual), FILENAME, __LINE__)) { \
            return false; \
        } \
    } while (0); \

#define UTIL_TEST_ASSERT_STR(exp, actual) \
    do { \
        if (!util_test_assert_str(exp, actual, STRINGIFY(exp), STRINGIFY(actual), FILENAME, __LINE__)) { \
            return false; \
        } \
    } while (0); \

#define UTIL_TEST_ASSERT_DATA(exp, actual, len) \
    do { \
        if (!util_test_assert_data(exp, actual, len, STRINGIFY(exp), STRINGIFY(actual), FILENAME, __LINE__)) { \
            return false; \
        } \
    } while (0); \

typedef bool (*util_test_func_t)();

void util_test_init();

void util_test_add(const char* name, util_test_func_t func);

void util_test_exec();

void util_test_shutdown();

bool util_test_assert_exp(bool exp, const char* exp_str, const char* file, size_t line_num);

bool util_test_assert_uint(size_t val_exp, size_t val_actual, const char* exp, const char* actual, const char* file,
    size_t line_num);

bool util_test_assert_str(const char* val_exp, const char* val_actual, const char* exp, const char* actual,
    const char* file, size_t line_num);

bool util_test_assert_data(const void* data_exp, const void* data_actual, size_t len, const char* exp,
    const char* actual, const char* file, size_t line_num);

#endif
