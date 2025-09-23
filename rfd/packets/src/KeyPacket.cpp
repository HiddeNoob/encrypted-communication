#include "KeyPacket.h"
#include <cstring>
#include "AckPacket.h"

// KeyPacket

void KeyPacket::serialize(uint8_t* buffer) const {
    Packet::serialize(buffer);
    size_t offset = Packet::getPayloadSize(); // base payload kadar ilerle
    memcpy(buffer + offset, &target_id, sizeof(target_id));
    offset += sizeof(target_id);
    memcpy(buffer + offset, &public_key, sizeof(public_key));
    offset += sizeof(public_key);
    memcpy(buffer + offset, &modulus, sizeof(modulus));
}

bool KeyPacket::deserialize(const uint8_t* buffer, uint8_t length) {
    if (length < getPayloadSize()) return false;
    Packet::deserialize(buffer, length);
    size_t offset = Packet::getPayloadSize();
    memcpy(&target_id, buffer + offset, sizeof(target_id));
    offset += sizeof(target_id);
    memcpy(&public_key, buffer + offset, sizeof(public_key));
    offset += sizeof(public_key);
    memcpy(&modulus, buffer + offset, sizeof(modulus));
    return true;
}

void KeyPacket::handle(Node& node_instance) {
    printf("[KeyPacket] Received KEY from: %llx to: %llx\n", sender_id, target_id);
    printf("[KeyPacket] Public Key: %llx, Modulus: %llx\n", public_key, modulus);
    if (target_id == node_instance.getId()) {
        printf("[KeyPacket] KEY bana gonderildi, ACK gonderiliyor\n");

        node_instance.registerNode(sender_id, public_key, modulus);

        auto ack = std::make_unique<AckPacket>(node_instance.getId(), sender_id, PACKET_KEY);
        ack->send(node_instance.getRFD().getUart());
    } else {
        printf("[KeyPacket] KEY baska node'a gonderildi, gormezden geliniyor\n");
        return; // burada fonksiyondan cikiyoruz, ack gondermiyor
    }
}