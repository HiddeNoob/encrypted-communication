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
    RSA(rsa_data tot,rsa_data n);

    ~RSA();
    void Encrypt(const char* data, int data_len, rsa_data* encrypted);
    void Decrypt(const rsa_data* encrypted, int data_len, char* decrypted);
    
    // Getter fonksiyonları
    rsa_data getPublicKey() const { return public_key; }
    rsa_data getPrivateKey() const { return private_key; }
    rsa_data getModulus() const { return n; }

private:
    void GenerateKeys(char bits);
    void GenerateKeys(rsa_data tot, rsa_data n);
    rsa_data encryptByte(char byte);
    rsa_data decryptByte(rsa_data encrypted_value);

    rsa_data p, q, n, tot;
    rsa_data public_key, private_key;
};
