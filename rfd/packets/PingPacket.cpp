#include "PingPacket.h"
#include "AckPacket.h"
#include <cstring>

// PingPacket

void PingPacket::serialize(uint8_t* buffer) const {
    Packet::serialize(buffer);
    size_t offset = Packet::getPayloadSize(); // base payload kadar ilerle
    memcpy(buffer + offset, &target_id, sizeof(target_id));
    offset += sizeof(target_id);
    memcpy(buffer + offset, &timestamp, sizeof(timestamp));
}

bool PingPacket::deserialize(const uint8_t* buffer, uint8_t length) {
    if (length < getPayloadSize()) return false;
    Packet::deserialize(buffer, length);
    size_t offset = Packet::getPayloadSize();
    memcpy(&sender_id, buffer + offset, sizeof(sender_id));
    offset += sizeof(sender_id);
    memcpy(&target_id, buffer + offset, sizeof(target_id));
    offset += sizeof(target_id);
    memcpy(&timestamp, buffer + offset, sizeof(timestamp));
    return true;
}

void PingPacket::handle(Node& node_instance) {
    printf("[PingPacket] Received PING from: %llx to: %llx, timestamp: %u\n", 
           sender_id, target_id, timestamp);
    
    // sadece bana gonderilen ping'lere cevap ver
    if (target_id == node_instance.getId()) {
        printf("[PingPacket] PING bana gonderildi, ACK gonderiliyor\n");
        auto ack = std::make_unique<AckPacket>(node_instance.getId(), target_id, PACKET_PING);
        ack->send(node_instance.getRFD().getUart());
    } else {
        printf("[PingPacket] PING baska node'a gonderildi, gormezden geliniyor\n");
        return; // burada fonksiyondan cikiyoruz, ack gondermiyor
    }
}