#include "RSA.h"
#include "FastMath.h"
#include "PrimeGenerator.h"
#include <ctime>

RSA::RSA(int bits) {
    GenerateKeys(bits);
}


RSA::RSA(rsa_data tot,rsa_data n){
    GenerateKeys(tot,n);
}

RSA::~RSA() {
    // Şimdilik boşrt
}


void RSA::GenerateKeys(rsa_data tot,rsa_data n){
    rsa_data public_key;
    rsa_data private_key;
    
    do {
        srand(time(NULL)); // kesinlikle güvenli değil
        public_key = rand() % (tot - 2) + 2;
    } while (FastMath::gcd(tot, public_key) != 1);

    private_key = FastMath::mod_inverse(public_key, tot);
    
    printf("--Public Key--\n%llx\n", public_key);
    printf("\n");
    printf("--Private Key--\n%llx\n", private_key);

    if((private_key * public_key) % tot != 1) {
        printf("RSA key generation error: private_key * public_key mod tot != 1\n");
        abort();
    }

    this->private_key = new Key(private_key,n);
    this->public_key = new Key(public_key,n);


    printf("keys generated successfully.\n");

}

void RSA::GenerateKeys(char bits) {
    //srand(time(NULL));
    rsa_data limit = ((rsa_data)1 << (rsa_data)bits) - 1;
    PrimeGenerator primeGen(limit);

    printf("[RSA] Generating keys (%d bits)...\n", bits);

    rsa_data p = primeGen.getRandomPrime();
    rsa_data q = primeGen.getRandomPrime();
    rsa_data n = (rsa_data)p * q;
    rsa_data tot = FastMath::totient(p, q);
    printf("[RSA] Selected primes p=%llu, q=%llu\n", p, q);
    printf("[RSA] Calculated n=%llu and totient=%llu\n", n, tot);

    this->GenerateKeys(tot,n);
}
