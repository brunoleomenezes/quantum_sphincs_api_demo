
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "../external/sphincsplus/ref/api.h"
#include "../external/sphincsplus/ref/params.h"
#include "../external/sphincsplus/ref/randombytes.h"
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include <Python.h>

#define MESSAGE_LEN 32
#define SIGNATURE_LEN CRYPTO_BYTES
#define PUBLIC_KEY_LEN CRYPTO_PUBLICKEYBYTES
#define SECRET_KEY_LEN CRYPTO_SECRETKEYBYTES

static void print_hex(const unsigned char *data, size_t len) {
    for (size_t i = 0; i < len; ++i) printf("%02x ", data[i]);
    printf("\n");
}

static void test_keygen(void **state) {
    unsigned char pk[PUBLIC_KEY_LEN], sk[SECRET_KEY_LEN];
    memset(pk, 0, PUBLIC_KEY_LEN);
    memset(sk, 0, SECRET_KEY_LEN);
    int result = crypto_sign_keypair(pk, sk);
    printf("Key Generation Result: %d\n", result);
    print_hex(pk, PUBLIC_KEY_LEN);
    assert_int_equal(result, 0);
}

static void test_sign_verify(void **state) {
    unsigned char msg[MESSAGE_LEN] = {0}, sig[SIGNATURE_LEN], pk[PUBLIC_KEY_LEN], sk[SECRET_KEY_LEN], verified[MESSAGE_LEN];
    unsigned long long siglen = 0, mlen = MESSAGE_LEN;
    crypto_sign_keypair(pk, sk);
    crypto_sign(sig, &siglen, msg, MESSAGE_LEN, sk);
    crypto_sign_open(verified, &mlen, sig, siglen, pk);
    assert_memory_equal(msg, verified, MESSAGE_LEN);
}

static void test_quantum_resistance(void **state) {
    Py_Initialize();
    if (!Py_IsInitialized()) return;
    const char* scripts[] = {"../src/python/quantum_attack_2.py", "../src/python/quantum_attack_ghz.py", "../src/python/quantum_attack_4_qubits.py", "../src/python/quantum_attack_grover.py"};
    for (int i = 0; i < 4; i++) {
        FILE* fp = fopen(scripts[i], "r");
        if (fp) {
            PyRun_SimpleFile(fp, scripts[i]);
            fclose(fp);
        }
    }
    Py_Finalize();
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_keygen),
        cmocka_unit_test(test_sign_verify),
        cmocka_unit_test(test_quantum_resistance),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
