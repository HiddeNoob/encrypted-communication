#pragma once
#include <string>
#include "PrimeGenerator.h"
#include "FastMath.h"
#include "config.h"


// https://www.cryptool.org/en/cto/rsa-step-by-step/
// https://www.derspresso.com.tr/matematik/moduler-aritmetik
class RSA {
public:
    RSA(int bits);
    ~RSA();
    void Encrypt(const char* data, int data_len, type* encrypted);
    void Decrypt(const type* encrypted, int data_len, char* decrypted);

private:
    void GenerateKeys(char bits);
    type encryptByte(char byte);
    type decryptByte(type encrypted_value);

    type p, q, n, tot;
    type public_key, private_key;
};
