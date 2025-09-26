#include "HelloPacket.h"
#include "AckPacket.h"
#include "KeyPacket.h"
#include <cstring>

// HelloPacket
void HelloPacket::serialize(uint8_t* buffer) const {
    Packet::serialize(buffer);
    size_t offset = Packet::getPayloadSize(); // base payload kadar ilerle
    memcpy(buffer + offset, &public_key, sizeof(public_key));
    offset += sizeof(public_key);
    memcpy(buffer + offset, &modulus, sizeof(modulus));
}

bool HelloPacket::deserialize(const uint8_t* buffer, uint16_t length) {
    if (length < getPayloadSize()) return false;

    Packet::deserialize(buffer, length);
    size_t offset = Packet::getPayloadSize();
    
    memcpy(&public_key, buffer + offset, sizeof(public_key));
    offset += sizeof(public_key);
    memcpy(&modulus, buffer + offset, sizeof(modulus));
    return true;
}

void HelloPacket::handle(Node& node_instance) {
    printf("[HelloPacket] Received HELLO from: %llx\n", sender_id);
    
    node_instance.registerNode(sender_id, public_key, modulus);
    
    // ACK gönder
    auto ack = std::make_unique<AckPacket>(node_instance.getId(), sender_id, PACKET_HELLO);
    ack->send(node_instance.getRFD().getUart());
    // kendi key'ini geri gonder
    auto keyPacket = std::make_unique<KeyPacket>(node_instance.getId(), sender_id,
                                                node_instance.getRFD().getCryptionService().getPublicKey()->getKey(),
                                                node_instance.getRFD().getCryptionService().getPublicKey()->getBase());
    
    printf("[HelloPacket] Sending KEY packet back to %llx\n", sender_id);
    printf("Public Key: %llx, Modulus: %llx\n", 
           node_instance.getRFD().getCryptionService().getPublicKey()->getKey(),
           node_instance.getRFD().getCryptionService().getPublicKey()->getBase());
    
    keyPacket->send(node_instance.getRFD().getUart());
}
