#include "rsa/headers/RSA.h"
#include <iostream>
#include <cstdint>
#include <AllPackets.h>
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

void test_serialization(){
    cout << "Testing PingPacket serialization..." << endl;
    PingPacket ping(0x1234567890abcdef, 0xfedcba0987654321, 0xAABBCCDD);
    uint8_t buffer[256];
    ping.serialize(buffer);
    PingPacket ping2(0,0,0);
    ping2.deserialize(buffer, ping.getPayloadSize());
    printf("Original PingPacket: sender_id=%llx, target_id=%llx, timestamp=%u\n", 
           ping.getSenderId(), ping.getTargetId(), ping.getTimestamp());
    printf("Deserialized PingPacket: sender_id=%llx, target_id=%llx, timestamp=%u\n", 
           ping2.getSenderId(), ping2.getTargetId(), ping2.getTimestamp());
    
}

int main(){
    test_cryption();
    test_serialization();
    return 0;
}