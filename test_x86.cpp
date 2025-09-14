#include "rsa/headers/RSA.h"
#include <iostream>
#include <cstdint>
#include <cstddef>

using namespace std;

void test_cryption(){
    cout << "Testing RSA encryption/decryption..." << endl;
    RSA rsa(16); // çok uzun sürüyor tüm asalları hesaplıyor 2^16'ya kadar, olasılık ile asal seçme algosuyla değiştirilebilir
    rsa_data* encrypted = rsa.getPublicKey()->sign((uint8_t*)"Hello, World!", 14, sizeof(char));
    rsa_data* decrypted = rsa.getPrivateKey()->sign(encrypted, 14, sizeof(rsa_data));
    printf("Encrypted: ");
    for(int i=0; i < 14;i++){
        printf("%lx ", encrypted[i] );
    }
    printf("\nDecrypted: ");
    for(int i=0; i < 14;i++){
        printf("%c", static_cast<char>(decrypted[i] & 0xFF));
    }
    printf("\n\n");
}

int main(){
    test_cryption();
    return 0;
}