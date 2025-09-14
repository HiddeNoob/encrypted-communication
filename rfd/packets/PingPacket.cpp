#include "PingPacket.h"
#include "AckPacket.h"
#include <cstring>

// PingPacket

void PingPacket::serialize(uint8_t* buffer) const {
    Packet::serialize(buffer);
    size_t offset = Packet::getPayloadSize(); // base payload kadar ilerle
    memcpy(buffer + offset, &target_id, sizeof(target_id));
    offset += sizeof(target_id);
}

bool PingPacket::deserialize(const uint8_t* buffer, uint8_t length) {
    if (length < getPayloadSize()) return false;
    Packet::deserialize(buffer, length);
    size_t offset = Packet::getPayloadSize();
    memcpy(&target_id, buffer + offset, sizeof(target_id));
    return true;
}

void PingPacket::handle(Node& node_instance) {
    
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