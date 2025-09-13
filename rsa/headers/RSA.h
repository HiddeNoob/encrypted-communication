#pragma once
#include <string>
#include "PrimeGenerator.h"
#include "FastMath.h"
#include "config.h"
#include "Key.h"


// https://www.cryptool.org/en/cto/rsa-step-by-step/
// https://www.derspresso.com.tr/matematik/moduler-aritmetik
class RSA {
public:
    RSA(int bits);
    RSA(rsa_data tot,rsa_data n);
    ~RSA();



    // Getter fonksiyonları
    Key* getPublicKey() const { return public_key; }
    Key* getPrivateKey() const { return private_key; }

private:
    void GenerateKeys(char bits);
    void GenerateKeys(rsa_data tot, rsa_data n);
    rsa_data encryptByte(char byte);
    rsa_data decryptByte(rsa_data encrypted_value);

    Key* public_key;
    Key* private_key;
};
