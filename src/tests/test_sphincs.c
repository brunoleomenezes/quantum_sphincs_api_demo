#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "../ref/api.h"
#include "../ref/params.h"
#include "../ref/randombytes.h"
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
	for (size_t i = 0; i < len; ++i) {
    	printf("%02x ", data[i]);
	}
	printf("\n");
}

/* Teste 1: Geração de chaves */
static void test_keygen(void **state) {
	unsigned char pk[PUBLIC_KEY_LEN];
	unsigned char sk[SECRET_KEY_LEN];

	memset(pk, 0, PUBLIC_KEY_LEN);
	memset(sk, 0, SECRET_KEY_LEN);

	int result = crypto_sign_keypair(pk, sk);
	printf("Key Generation Result: %d\n", result);

	printf("Public Key: ");
	print_hex(pk, PUBLIC_KEY_LEN);
	printf("Secret Key: ");
	print_hex(sk, SECRET_KEY_LEN);

	assert_int_equal(result, 0);
	assert_non_null(pk);
	assert_non_null(sk);
}

/* Teste 2: Assinatura e verificação */
static void test_sign_verify(void **state) {
	unsigned char message[MESSAGE_LEN] = {0};
	unsigned char signature[SIGNATURE_LEN];
	unsigned char pk[PUBLIC_KEY_LEN];
	unsigned char sk[SECRET_KEY_LEN];
	unsigned long long siglen = 0;

	memset(signature, 0, SIGNATURE_LEN);
	memset(pk, 0, PUBLIC_KEY_LEN);
	memset(sk, 0, SECRET_KEY_LEN);

	int keygen_result = crypto_sign_keypair(pk, sk);
	printf("Key Generation Result: %d\n", keygen_result);
	assert_int_equal(keygen_result, 0);

	int sign_result = crypto_sign(signature, &siglen, message, MESSAGE_LEN, sk);
	printf("Sign Result: %d\n", sign_result);
	printf("Signature: ");
	print_hex(signature, siglen);
	assert_int_equal(sign_result, 0);

	unsigned long long mlen = MESSAGE_LEN;
	unsigned char verified_message[MESSAGE_LEN];
	memset(verified_message, 0, MESSAGE_LEN);

	int verify_result = crypto_sign_open(verified_message, &mlen, signature, siglen, pk);
	printf("Verify Result: %d\n", verify_result);
	printf("Verified Message: ");
	print_hex(verified_message, mlen);

	assert_int_equal(verify_result, 0);
	assert_memory_equal(message, verified_message, MESSAGE_LEN);
}

/* Teste 3: Integração (geração, assinatura e verificação no mesmo fluxo) */
static void test_integration(void **state) {
	unsigned char message[MESSAGE_LEN] = {0};
	unsigned char signature[SIGNATURE_LEN];
	unsigned char pk[PUBLIC_KEY_LEN];
	unsigned char sk[SECRET_KEY_LEN];
	unsigned long long siglen = 0;

	memset(signature, 0, SIGNATURE_LEN);
	memset(pk, 0, PUBLIC_KEY_LEN);
	memset(sk, 0, SECRET_KEY_LEN);

	int keygen_result = crypto_sign_keypair(pk, sk);
	printf("Key Generation Result: %d\n", keygen_result);
	assert_int_equal(keygen_result, 0);

	int sign_result = crypto_sign(signature, &siglen, message, MESSAGE_LEN, sk);
	printf("Sign Result: %d\n", sign_result);
	printf("Signature: ");
	print_hex(signature, siglen);
	assert_int_equal(sign_result, 0);

	unsigned long long mlen = MESSAGE_LEN;
	unsigned char verified_message[MESSAGE_LEN];
	memset(verified_message, 0, MESSAGE_LEN);

	int verify_result = crypto_sign_open(verified_message, &mlen, signature, siglen, pk);
	printf("Verify Result: %d\n", verify_result);
	printf("Verified Message: ");
	print_hex(verified_message, mlen);

	assert_int_equal(verify_result, 0);
	assert_memory_equal(message, verified_message, MESSAGE_LEN);
}

/* Teste 4: Casos extremos e limites (mensagens muito curtas e muito longas) */
static void test_edge_cases(void **state) {
	unsigned char short_message[1] = {0};
	unsigned char long_message[1024] = {0};
	unsigned char signature[SIGNATURE_LEN];
	unsigned char pk[PUBLIC_KEY_LEN];
	unsigned char sk[SECRET_KEY_LEN];
	unsigned long long siglen = 0;

	memset(signature, 0, SIGNATURE_LEN);
	memset(pk, 0, PUBLIC_KEY_LEN);
	memset(sk, 0, SECRET_KEY_LEN);

	int keygen_result = crypto_sign_keypair(pk, sk);
	printf("Key Generation Result: %d\n", keygen_result);
	assert_int_equal(keygen_result, 0);

	/* Mensagem extremamente curta */
	int sign_result = crypto_sign(signature, &siglen, short_message, 1, sk);
	printf("Sign Result for Short Message: %d\n", sign_result);
	printf("Signature: ");
	print_hex(signature, siglen);
	assert_int_equal(sign_result, 0);

	unsigned long long mlen = 1;
	unsigned char verified_message[1] = {0};

	int verify_result = crypto_sign_open(verified_message, &mlen, signature, siglen, pk);
	printf("Verify Result for Short Message: %d\n", verify_result);
	printf("Verified Short Message: ");
	print_hex(verified_message, mlen);
	assert_int_equal(verify_result, 0);
	assert_memory_equal(short_message, verified_message, 1);

	/* Mensagem extremamente longa */
	sign_result = crypto_sign(signature, &siglen, long_message, 1024, sk);
	printf("Sign Result for Long Message: %d\n", sign_result);
	printf("Signature: ");
	print_hex(signature, siglen);
	assert_int_equal(sign_result, 0);

	mlen = 1024;
	unsigned char verified_long_message[1024] = {0};

	verify_result = crypto_sign_open(verified_long_message, &mlen, signature, siglen, pk);
	printf("Verify Result for Long Message: %d\n", verify_result);
	printf("Verified Long Message: ");
	print_hex(verified_long_message, mlen);
	assert_int_equal(verify_result, 0);
	assert_memory_equal(long_message, verified_long_message, 1024);
}

/* Teste 5: Entradas aleatórias (várias iterações) */
static void test_randomized(void **state) {
	unsigned char message[MESSAGE_LEN];
	unsigned char signature[SIGNATURE_LEN];
	unsigned char pk[PUBLIC_KEY_LEN];
	unsigned char sk[SECRET_KEY_LEN];
	unsigned long long siglen = 0;

	memset(signature, 0, SIGNATURE_LEN);
	memset(pk, 0, PUBLIC_KEY_LEN);
	memset(sk, 0, SECRET_KEY_LEN);

	for (int i = 0; i < 100; i++) {
    	randombytes(message, MESSAGE_LEN);

    	int keygen_result = crypto_sign_keypair(pk, sk);
    	printf("Key Generation Result: %d\n", keygen_result);
    	assert_int_equal(keygen_result, 0);

    	int sign_result = crypto_sign(signature, &siglen, message, MESSAGE_LEN, sk);
    	printf("Sign Result: %d\n", sign_result);
    	printf("Signature: ");
    	print_hex(signature, siglen);
    	assert_int_equal(sign_result, 0);

    	unsigned long long mlen = MESSAGE_LEN;
    	unsigned char verified_message[MESSAGE_LEN];
    	memset(verified_message, 0, MESSAGE_LEN);

    	int verify_result = crypto_sign_open(verified_message, &mlen, signature, siglen, pk);
    	printf("Verify Result: %d\n", verify_result);
    	printf("Verified Message: ");
    	print_hex(verified_message, mlen);

    	assert_int_equal(verify_result, 0);
    	assert_memory_equal(message, verified_message, MESSAGE_LEN);
	}
}

/* Teste 6: Respostas conhecidas */
static void test_known_answers(void **state) {
	unsigned char known_message[MESSAGE_LEN] = "Hello World! ";
	unsigned char signature[SIGNATURE_LEN];
	unsigned char pk[PUBLIC_KEY_LEN];
	unsigned char sk[SECRET_KEY_LEN];
	unsigned long long siglen = 0;

	memset(signature, 0, SIGNATURE_LEN);
	memset(pk, 0, PUBLIC_KEY_LEN);
	memset(sk, 0, SECRET_KEY_LEN);

	int keygen_result = crypto_sign_keypair(pk, sk);
	printf("Key Generation Result: %d\n", keygen_result);
	assert_int_equal(keygen_result, 0);

	int sign_result = crypto_sign(signature, &siglen, known_message, MESSAGE_LEN, sk);
	printf("Sign Result: %d\n", sign_result);
	printf("Signature: ");
	print_hex(signature, siglen);
	assert_int_equal(sign_result, 0);

	unsigned long long mlen = MESSAGE_LEN;
	unsigned char verified_message[MESSAGE_LEN];
	memset(verified_message, 0, MESSAGE_LEN);

	printf("Known Message: ");
	print_hex(known_message, MESSAGE_LEN);
	printf("Generated Signature: ");
	print_hex(signature, siglen);
	printf("Public Key: ");
	print_hex(pk, PUBLIC_KEY_LEN);

	int result = crypto_sign_open(verified_message, &mlen, signature, siglen, pk);
	printf("Verification Result: %d\n", result);

	if (result != 0) {
    	printf("Failed Verification Message: ");
    	print_hex(verified_message, mlen);
	}
	assert_int_equal(result, 0);
	assert_memory_equal(known_message, verified_message, MESSAGE_LEN);
}

/* Teste 7: Robustez frente a simulações quânticas */
static void test_quantum_resistance(void **state) {
	printf("Inicializando o Python...\n");
	Py_Initialize();

	if (!Py_IsInitialized()) {
    	printf("Failed to initialize Python interpreter\n");
    	return;
	}

	printf("Alocando memória...\n");
	unsigned char pk[PUBLIC_KEY_LEN];
	unsigned char sk[SECRET_KEY_LEN];
	unsigned char message[MESSAGE_LEN] = "Test message for SPHINCS+";
	unsigned char signature[SIGNATURE_LEN];
	unsigned long long siglen = 0;
	unsigned long long mlen = MESSAGE_LEN;
	unsigned char verified_message[MESSAGE_LEN];

	printf("Inicializando variáveis...\n");
	memset(pk, 0, PUBLIC_KEY_LEN);
	memset(sk, 0, SECRET_KEY_LEN);
	memset(signature, 0, SIGNATURE_LEN);
	memset(verified_message, 0, MESSAGE_LEN);

	printf("Gerando chaves...\n");
	int keygen_result = crypto_sign_keypair(pk, sk);
	printf("Key Generation Result: %d\n", keygen_result);
	if (keygen_result != 0) {
    	printf("Key generation failed.\n");
    	Py_Finalize();
    	return;
	}

	printf("Assinando mensagem...\n");
	int sign_result = crypto_sign(signature, &siglen, message, MESSAGE_LEN, sk);
	printf("Sign Result: %d\n", sign_result);
	if (sign_result != 0) {
    	printf("Signing failed.\n");
    	Py_Finalize();
    	return;
	}

	printf("Verificando assinatura...\n");
	int verify_result = crypto_sign_open(verified_message, &mlen, signature, siglen, pk);
	printf("Verify Result: %d\n", verify_result);
	if (verify_result != 0) {
    	printf("Verification failed.\n");
    	Py_Finalize();
    	return;
	}

	/* Lista de scripts (circuitos quânticos) a serem executados */
	const char* simulation_scripts[] = {
    	"../test/quantum_simulation_2.py",
    	"../test/quantum_simulation_ghz.py",
    	"../test/quantum_simulation_4_qubits.py",
    	"../test/quantum_simulation_grover.py"
	};

	const int num_attacks = sizeof(simulation_scripts) / sizeof(simulation_scripts[0]);

	for (int i = 0; i < num_attacks; i++) {
    	printf("Abrindo script Python %s...\n", simulation_scripts[i]);
    	FILE* fp = fopen(simulation_scripts[i], "r");
    	if (fp == NULL) {
        	printf("Failed to open %s\n", simulation_scripts[i]);
        	continue;
    	}
    	printf("Executando script Python %s...\n", simulation_scripts[i]);
    	PyRun_SimpleFile(fp, simulation_scripts[i]);
    	fclose(fp);

    	if (PyErr_Occurred()) {
        	PyErr_Print();
    	}
	}

	Py_Finalize();
	printf("Python finalizado.\n");

	printf("Gerando chaves...\n");
	memset(pk, 0, PUBLIC_KEY_LEN);
	memset(sk, 0, SECRET_KEY_LEN);
	memset(signature, 0, SIGNATURE_LEN);
	memset(verified_message, 0, MESSAGE_LEN);

	keygen_result = crypto_sign_keypair(pk, sk);
	printf("Key Generation Result: %d\n", keygen_result);

	printf("Assinando mensagem...\n");
	sign_result = crypto_sign(signature, &siglen, message, MESSAGE_LEN, sk);
	printf("Sign Resul: %d\n", sign_result);

	printf("Verificando assinatura...\n");
	verify_result = crypto_sign_open(verified_message, &mlen, signature, siglen, pk);
	printf("Verify Result: %d\n", verify_result);
}

int main(void) {
	const struct CMUnitTest tests[] = {
    	cmocka_unit_test(test_keygen),
    	cmocka_unit_test(test_sign_verify),
    	cmocka_unit_test(test_integration),
    	cmocka_unit_test(test_edge_cases),
    	cmocka_unit_test(test_randomized),
    	cmocka_unit_test(test_known_answers),
    	cmocka_unit_test(test_quantum_resistance),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}
