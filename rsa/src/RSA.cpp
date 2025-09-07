#include "RSA.h"
#include "FastMath.h"
#include "PrimeGenerator.h"

RSA::RSA(int bits) {
    GenerateKeys(bits);
}

RSA::~RSA() {
    // Şimdilik boşrt
}

void RSA::Encrypt(const char* data, int data_len, type* encrypted) {
    for (int i = 0; i < data_len; ++i) {
        encrypted[i] = encryptByte(data[i]);
    }
}

void RSA::Decrypt(const type* encrypted, int data_len, char* decrypted) {
    for (int i = 0; i < data_len; ++i) {
        decrypted[i] = (char)decryptByte(encrypted[i]);
    }
}

type RSA::encryptByte(char byte) {
    return FastMath::fast_pow(byte, public_key, n);
}

type RSA::decryptByte(type encrypted_value) {
    return FastMath::fast_pow(encrypted_value, private_key, n);
}

void RSA::GenerateKeys(char bits) {
    //srand(time(NULL));
    type limit = (type)1 << (type)bits;
    PrimeGenerator primeGen(limit);

    printf("[RSA] Generating keys (%d bits)...\n", bits);

    p = primeGen.getRandomPrime();
    q = primeGen.getRandomPrime();
    n = (type)p * q;
    tot = FastMath::totient(p, q);

    do {
        public_key = rand() % (tot - 2) + 2;
    } while (FastMath::gcd(tot, public_key) != 1);

    private_key = FastMath::mod_inverse(public_key, tot);

    if((private_key * public_key) % tot != 1) {
        printf("RSA key generation error: private_key * public_key mod tot != 1\n");
        // Hata yönetimi: burada return veya başka bir mekanizma eklenebilir
    }

    printf("keys generated successfully.\n");
    printf("--Private Key--\n%llx", public_key), 
    printf("\n");
    printf("--Public Key--\n%llx\n", private_key);
}
