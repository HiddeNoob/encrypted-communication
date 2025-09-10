#include "Packet.h"
#include "RFD.h"
#include <cstring>
#include <cstdio>

// Ana Packet sınıfı metodları
void Packet::send(uart_inst_t* uart) const {
    PacketHeader header;
    header.magic1 = 0xAA;
    header.magic2 = 0x55;
    header.type = (uint8_t)packet_type;
    header.length = getPayloadSize();

    // Header gönder
    uint8_t* header_bytes = (uint8_t*)&header;
    for (int i = 0; i < sizeof(PacketHeader); i++) {
        uart_putc_raw(uart, header_bytes[i]);
    }

    // Payload gönder
    uint8_t payload_buffer[256];
    serialize(payload_buffer);
    for (int i = 0; i < header.length; i++) {
        uart_putc_raw(uart, payload_buffer[i]);
    }

    uart_tx_wait_blocking(uart);
    printf("[Packet] Sent %s packet (size: %d)\n", 
           packet_type == PACKET_HELLO ? "HELLO" :
           packet_type == PACKET_SIGNED_MESSAGE ? "MESSAGE" :
           packet_type == PACKET_PING ? "PING" :
           packet_type == PACKET_ACK ? "ACK" : "UNKNOWN", 
           header.length);
}

std::unique_ptr<Packet> Packet::createFromBuffer(const uint8_t* buffer, uint8_t length, PacketType type) {
    switch (type) {
        case PACKET_HELLO: {
            auto packet = std::make_unique<HelloPacket>(0, 0, 0);
            if (packet->deserialize(buffer, length)) {
                return std::move(packet);
            }
            break;
        }
        case PACKET_SIGNED_MESSAGE: {
            auto packet = std::make_unique<SignedMessagePacket>(0, 0);
            if (packet->deserialize(buffer, length)) {
                return std::move(packet);
            }
            break;
        }
        case PACKET_PING: {
            auto packet = std::make_unique<PingPacket>(0, 0, 0);
            if (packet->deserialize(buffer, length)) {
                return std::move(packet);
            }
            break;
        }
        case PACKET_ACK: {
            auto packet = std::make_unique<AckPacket>(0, 0);
            if (packet->deserialize(buffer, length)) {
                return std::move(packet);
            }
            break;
        }
        default:
            printf("[Packet] Unknown packet type: 0x%02X\n", type);
            break;
    }
    return nullptr;
}

// HelloPacket
void HelloPacket::serialize(uint8_t* buffer) const {
    memcpy(buffer, &sender_id, sizeof(uint64_t));
    memcpy(buffer + sizeof(uint64_t), &public_key, sizeof(uint64_t));
    memcpy(buffer + 2 * sizeof(uint64_t), &modulus, sizeof(uint64_t));
}

bool HelloPacket::deserialize(const uint8_t* buffer, uint8_t length) {
    if (length < getPayloadSize()) return false;
    
    memcpy(&sender_id, buffer, sizeof(uint64_t));
    memcpy(&public_key, buffer + sizeof(uint64_t), sizeof(uint64_t));
    memcpy(&modulus, buffer + 2 * sizeof(uint64_t), sizeof(uint64_t));
    return true;
}

void HelloPacket::handle(RFD* rfd_instance) {
    printf("[HelloPacket] Received HELLO from: %llx\n", sender_id);
    rfd_instance->registerNode(sender_id, public_key, modulus);
    
    // ACK gönder
    auto ack = std::make_unique<AckPacket>(rfd_instance->getMyId(), PACKET_HELLO);
    ack->send(rfd_instance->getUart());
}

// SignedMessagePacket
void SignedMessagePacket::serialize(uint8_t* buffer) const {
    size_t offset = 0;
    memcpy(buffer + offset, &sender_id, sizeof(uint64_t));
    offset += sizeof(uint64_t);
    memcpy(buffer + offset, &target_id, sizeof(uint64_t));
    offset += sizeof(uint64_t);
    memcpy(buffer + offset, payload, 32);
    offset += 32;
    memcpy(buffer + offset, signature, 8);
}

bool SignedMessagePacket::deserialize(const uint8_t* buffer, uint8_t length) {
    if (length < getPayloadSize()) return false;
    
    size_t offset = 0;
    memcpy(&sender_id, buffer + offset, sizeof(uint64_t));
    offset += sizeof(uint64_t);
    memcpy(&target_id, buffer + offset, sizeof(uint64_t));
    offset += sizeof(uint64_t);
    memcpy(payload, buffer + offset, 32);
    offset += 32;
    memcpy(signature, buffer + offset, 8);
    return true;
}

void SignedMessagePacket::handle(RFD* rfd_instance) {
    printf("[SignedMessagePacket] Received message from: %llx to: %llx\n", 
           sender_id, target_id);
    
    if (target_id == rfd_instance->getMyId()) {
        printf("[SignedMessagePacket] Message for me! Payload: ");
        for (int i = 0; i < 32 && payload[i] != 0; i++) {
            printf("%c", payload[i]);
        }
        printf("\n");
        
        // ACK gönder
        auto ack = std::make_unique<AckPacket>(rfd_instance->getMyId(), PACKET_SIGNED_MESSAGE);
        ack->send(rfd_instance->getUart());
    } else {
        printf("[SignedMessagePacket] Message not for me, ignoring\n");
    }
}

void SignedMessagePacket::setPayload(const uint8_t* data, size_t len) {
    size_t copy_len = (len > 32) ? 32 : len;
    memset(payload, 0, 32);
    memcpy(payload, data, copy_len);
}

void SignedMessagePacket::setSignature(const uint8_t* sig) {
    memcpy(signature, sig, 8);
}

// PingPacket

void PingPacket::serialize(uint8_t* buffer) const {
    memcpy(buffer, &sender_id, sizeof(uint64_t));
    memcpy(buffer + sizeof(uint64_t), &target_id, sizeof(uint64_t));
    memcpy(buffer + sizeof(uint64_t) * 2, &timestamp, sizeof(uint32_t));
}

bool PingPacket::deserialize(const uint8_t* buffer, uint8_t length) {
    if (length < getPayloadSize()) return false;
    
    memcpy(&sender_id, buffer, sizeof(uint64_t));
    memcpy(&target_id, buffer + sizeof(uint64_t), sizeof(uint64_t));
    memcpy(&timestamp, buffer + sizeof(uint64_t) * 2, sizeof(uint32_t));
    return true;
}

void PingPacket::handle(RFD* rfd_instance) {
    printf("[PingPacket] Received PING from: %llx to: %llx, timestamp: %u\n", 
           sender_id, target_id, timestamp);
    
    // sadece bana gonderilen ping'lere cevap ver
    if (target_id == rfd_instance->getMyId()) {
        printf("[PingPacket] PING bana gonderildi, ACK gonderiliyor\n");
        auto ack = std::make_unique<AckPacket>(rfd_instance->getMyId(), PACKET_PING);
        ack->send(rfd_instance->getUart());
    } else {
        printf("[PingPacket] PING baska node'a gonderildi, gormezden geliniyor\n");
        return; // burada fonksiyondan cikiyoruz, ack gondermiyor
    }
}

// AckPacket implementasyonu
void AckPacket::serialize(uint8_t* buffer) const {
    memcpy(buffer, &sender_id, sizeof(uint64_t));
    memcpy(buffer + sizeof(uint64_t), &ack_type, sizeof(uint8_t));
}

bool AckPacket::deserialize(const uint8_t* buffer, uint8_t length) {
    if (length < getPayloadSize()) return false;
    
    memcpy(&sender_id, buffer, sizeof(uint64_t));
    memcpy(&ack_type, buffer + sizeof(uint64_t), sizeof(uint8_t));
    return true;
}

void AckPacket::handle(RFD* rfd_instance) {
    printf("[AckPacket] Received ACK from: %llx for packet type: 0x%02X\n", 
           sender_id, ack_type);
}
