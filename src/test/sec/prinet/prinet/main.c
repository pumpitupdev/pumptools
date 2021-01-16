#include <cmocka/cmocka.h>

#include "sec/prinet/prinet.h"

#include "test-util/mem.h"

#include "util/mem.h"
#include "util/rand.h"

/* Compiled binary data from data folder. Symbols are defined by compiler */
extern const uint8_t _binary_prime_private_key_start[];
extern const uint8_t _binary_prime_private_key_end[];
extern const uint8_t _binary_prime_public_key_start[];
extern const uint8_t _binary_prime_public_key_end[];

static void _test_encrypt_decrypt(uint32_t seed)
{
    util_rand_init(seed);

    uint8_t* data;
    size_t data_len;
    uint8_t* nounce;
    uint8_t* enc_data;
    size_t enc_data_len;
    uint8_t* dec_data;
    size_t dec_data_len;
    size_t enc_data_len_res;
    size_t dec_data_len_res;

    for (uint32_t i = 0; i < 20; i++) {
        data_len = util_rand_gen_range_32(1024 * 1024);
        data = util_xmalloc(data_len);
        util_rand_gen_bytes(data, data_len);

        nounce = util_xmalloc(SEC_PRINET_NOUNCE_LEN);
        util_rand_gen_bytes(nounce, SEC_PRINET_NOUNCE_LEN);

        enc_data_len = sec_prinet_get_enc_data_buffer_size(data_len);
        enc_data = util_xmalloc(enc_data_len);

        enc_data_len_res =
            sec_prinet_encrypt(nounce, SEC_PRINET_NOUNCE_LEN, data, data_len, enc_data);

        assert_int_equal(enc_data_len_res, enc_data_len);

        dec_data_len = enc_data_len_res;
        dec_data = util_xmalloc(dec_data_len);

        dec_data_len_res =
            sec_prinet_decrypt(nounce, SEC_PRINET_NOUNCE_LEN, enc_data, dec_data_len, dec_data);

        assert_int_equal(dec_data_len_res, sec_prinet_get_enc_data_buffer_size(data_len));

        assert_memory_equal(data, dec_data, data_len);

        util_xfree((void**) &dec_data);
        util_xfree((void**) &enc_data);
        util_xfree((void**) &nounce);
        util_xfree((void**) &data);
    }
}

static int setup(void** state)
{
    test_util_mem_install_mem_interface();

    sec_prinet_init(
        (const uint8_t*) _binary_prime_public_key_start,
        ((uintptr_t) &_binary_prime_public_key_end -
        (uintptr_t) &_binary_prime_public_key_start),
        (const uint8_t*) _binary_prime_private_key_start,
        ((uintptr_t) &_binary_prime_private_key_end -
        (uintptr_t) &_binary_prime_private_key_start));

    return 0;
}

static int teardown(void** state)
{
    sec_prinet_finit();

    return 0;
}

static void test_encrypt_decrypt_1(void** state)
{
    _test_encrypt_decrypt(1);
}

static void test_encrypt_decrypt_2(void** state)
{
    _test_encrypt_decrypt(2);
}

static void test_encrypt_decrypt_3(void** state)
{
    _test_encrypt_decrypt(3);
}

static void test_encrypt_decrypt_4(void** state)
{
    _test_encrypt_decrypt(4);
}

int main(int argc, char* argv[])
{
    const struct CMUnitTest tests[] ={
        cmocka_unit_test_setup_teardown(test_encrypt_decrypt_1, setup, teardown),
        cmocka_unit_test_setup_teardown(test_encrypt_decrypt_2, setup, teardown),
        cmocka_unit_test_setup_teardown(test_encrypt_decrypt_3, setup, teardown),
        cmocka_unit_test_setup_teardown(test_encrypt_decrypt_4, setup, teardown)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}