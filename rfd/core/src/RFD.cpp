#include "RFD.h"
#include "AllPackets.h"
#include "Key.h"
#include <cstring>
#include <cstdio>

RFD::RFD(uart_inst_t* uart_inst, uint tx_pin, uint rx_pin) 
    : uart(uart_inst, tx_pin, rx_pin), cryptionService(16) { // 16 bitlik RSA anahtarı oluştur
        initID();
        packet_handler = std::make_unique<PacketHandler>(uart);
    }


void RFD::initID(){
    // Rastgele ID oluştur (gerçek uygulamada benzersiz ID kullanılması önerilir)
    srand(time_us_32());
    id = ((uint64_t)rand() << 32) | rand();
    printf("[RFD] Node ID initialized: %llx\n", id);
}

void RFD::processIncomingData(Node& node) {
    PacketHeader header;
    uint8_t payload[MAX_PACKET_PAYLOAD_SIZE];
    
    if (packet_handler->receivePacket(&header, payload)) {
        printf("[RFD %llx] Received packet type: 0x%02X, length: %d\n", 
               getId(), header.type, header.length);
        
        // Paketi oluştur ve işle
        auto packet = Packet::createFromBuffer(payload, header.length, (PacketType)header.type);
        if (packet) {
            packet->handle(node);
        } else {
            printf("[RFD %llx] Failed to create packet from buffer\n", getId());
        }
    }
    
}

void RFD::sendHello() {
    auto hello = std::make_unique<HelloPacket>(getId(), 
                                               getCryptionService().getPublicKey()->getKey(),
                                               getCryptionService().getPublicKey()->getBase());
    printf("[RFD %llx] Sending HELLO packet\n", getId());
    printf("Public Key: %llx, Modulus: %llx\n", 
           getCryptionService().getPublicKey()->getKey(),
           getCryptionService().getPublicKey()->getBase());
    hello->send(getUart());
}

void RFD::sendMessage(uint64_t target_id, const char* message) {
    printf("[RFD %llx] Starting message send to %llx\n", this->getId(), target_id);
    
    auto msg = std::make_unique<SignedMessagePacket>(getId(), target_id);
    
    uint32_t message_length = strlen(message);
    printf("[RFD %llx] Message length: %u\n", this->getId(), message_length);

    
    // Mesajı şifrele
    printf("[RFD %llx] Looking for target node...\n", this->getId());
    RemoteNode* targetNode = connectedNode->getNode(target_id);
    if (!targetNode) {
        printf("[RFD %llx] Hata: Hedef node bulunamadi: %llx\n", this->getId(), target_id);
        return;
    }

    Key targetPublicKey = targetNode->getPublicKey();
    printf("[RFD %llx] Target node %llx found, using public key: %llx, base: %llx\n", 
           this->getId(), target_id, targetPublicKey.getKey(), targetPublicKey.getBase());
    
    printf("[RFD %llx] Starting encryption...\n", this->getId());
    rsa_data* encrypted_data = targetPublicKey.sign((void*)message,message_length,sizeof(char));
    printf("[RFD %llx] Encryption completed\n", this->getId());

    // Mesajı payload'a kopyala
    msg->setPayload(encrypted_data, message_length * sizeof(rsa_data));
    delete[] encrypted_data;
    printf("[RFD %llx] Payload set and encrypted_data cleaned\n", this->getId());


    // Basit imza oluştur
    printf("[RFD %llx] Creating hash signature...\n", this->getId());
    uint32_t signature[message_length];
    FastMath::myHash((uint8_t*)message,signature,message_length);
    printf("[RFD %llx] Hash created\n", this->getId());

    
    // hashi imzala
    printf("[RFD %llx] Signing hash...\n", this->getId());
    Key* myPrivateKey = getCryptionService().getPrivateKey();
    rsa_data* signedMessage = myPrivateKey->sign((void*)signature, message_length, sizeof(uint32_t));
    printf("[RFD %llx] Hash signed\n", this->getId());
    
    msg->setSignature(signedMessage, message_length * sizeof(uint32_t));
    delete[] signedMessage;
    
    printf("[RFD %llx] Sending message to: %llx\n", this->getId(), target_id);
    msg->send(uart);
    printf("[RFD %llx] Message sent successfully!\n", this->getId());
}

void RFD::sendPing(uint64_t target_id) {
    printf("[RFD %llx] Creating PING packet...\n", this->getId());
    auto ping = std::make_unique<PingPacket>(this->getId(), target_id, time_us_32());
    printf("[RFD %llx] Sending PING to: %llx\n", this->getId(), target_id);
    ping->send(uart);
    printf("[RFD %llx] PING sent successfully!\n", this->getId());
}
