#pragma once
#include <string>
#include "PrimeGenerator.h"
#include "FastMath.h"


// https://www.cryptool.org/en/cto/rsa-step-by-step/
// https://www.derspresso.com.tr/matematik/moduler-aritmetik
class RSA {
public:
    RSA(int bits);
    ~RSA();
    void Encrypt(const char* data, int data_len, __uint128_t* encrypted);
    void Decrypt(const __uint128_t* encrypted, int data_len, char* decrypted);

private:
    void GenerateKeys(char bits);
    __uint128_t encryptByte(char byte);
    __uint128_t decryptByte(__uint128_t encrypted_value);

    __uint128_t p, q, n, tot;
    __uint128_t public_key, private_key;
};
