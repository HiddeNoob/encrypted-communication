#include "RSA.h"
#include "FastMath.h"
#include "PrimeGenerator.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cstring>

RSA::RSA(int bits) {
    GenerateKeys(bits);
}

RSA::~RSA() {
    // Şimdilik boşrt
}

void RSA::Encrypt(const char* data, int data_len, __uint128_t* encrypted) {
    for (int i = 0; i < data_len; ++i) {
        encrypted[i] = encryptByte(data[i]);
    }
}

void RSA::Decrypt(const __uint128_t* encrypted, int data_len, char* decrypted) {
    for (int i = 0; i < data_len; ++i) {
        decrypted[i] = (char)decryptByte(encrypted[i]);
    }
}

__uint128_t RSA::encryptByte(char byte) {
    return FastMath::fast_pow(byte, public_key, n);
}

__uint128_t RSA::decryptByte(__uint128_t encrypted_value) {
    return FastMath::fast_pow(encrypted_value, private_key, n);
}

void RSA::GenerateKeys(char bits) {
    srand(time(NULL));
    __uint128_t limit = (__uint128_t)1 << (__uint128_t)bits;
    PrimeGenerator primeGen(limit);

    printf("[RSA] Generating keys (%d bits)...\n", bits);

    p = primeGen.getRandomPrime();
    q = primeGen.getRandomPrime();
    n = (__uint128_t)p * q;
    tot = FastMath::totient(p, q);

    do {
        public_key = rand() % (tot - 2) + 2;
    } while (FastMath::gcd(tot, public_key) != 1);

    private_key = FastMath::mod_inverse(public_key, tot);

    if((private_key * public_key) % tot != 1) throw -1;

    printf("keys generated successfully.\n");
    printf("--Private Key--\n%016llx%016llx", 
           (unsigned long long)(public_key >> 64), 
           (unsigned long long)(public_key & 0xFFFFFFFFFFFFFFFF));
    printf("\n");
    printf("--Public Key--\n%016llx%016llx\n", 
           (unsigned long long)(private_key >> 64), 
           (unsigned long long)(private_key & 0xFFFFFFFFFFFFFFFF));
}
