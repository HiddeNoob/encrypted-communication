#include "pico/stdio.h"
#include "AckPacket.h"
#include <cstdio>
#include <cstring>


// AckPacket implementasyonu
void AckPacket::serialize(uint8_t* buffer) const {
    Packet::serialize(buffer);
    size_t offset = Packet::getPayloadSize(); // base payload kadar ilerle
    memcpy(buffer + offset, &ack_type, sizeof(ack_type));
    offset += sizeof(ack_type);
    memcpy(buffer + offset, &receiver, sizeof(receiver));
    offset += sizeof(receiver);
}

bool AckPacket::deserialize(const uint8_t* buffer, uint16_t length) {
    if (length < getPayloadSize()) return false;
    Packet::deserialize(buffer, length);
    size_t offset = Packet::getPayloadSize();
    memcpy(&ack_type, buffer + offset, sizeof(ack_type));
    offset += sizeof(ack_type);
    memcpy(&receiver, buffer + offset, sizeof(receiver));
    return true;
}

void AckPacket::handle(Node& node_instance) {
    printf("[AckPacket] Received ACK from: %llx for packet type: 0x%02X\n", 
           sender_id, ack_type);
}