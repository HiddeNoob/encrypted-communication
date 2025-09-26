#include "AllPackets.h"
#include "rfd_config.h"

void Packet::serialize(uint8_t* buffer) const {
    memcpy(buffer, &sender_id, sizeof(sender_id));
}

bool Packet::deserialize(const uint8_t* buffer, uint16_t length) {
    if (length < getPayloadSize()) return false;

    memcpy(&sender_id, buffer, sizeof(sender_id));
    return true;
}


// Ana Packet sınıfı metodları
void Packet::send(IComm& comm) const {
    PacketHeader header;
    header.magic1 = 0xAA;
    header.magic2 = 0x55;
    header.type = (uint8_t)packet_type;
    header.length = getPayloadSize();
    printf("[Packet] Preparing to send packet type: 0x%02X, length: %d\n", header.type, header.length);
    if (RFD_ENABLE_PACKET_HEX_LOG) {
        printf("[Packet] Header bytes: ");
        for (int i = 0; i < (int)sizeof(PacketHeader); i++) {
            printf("%02X ", ((uint8_t*)&header)[i]);
        }
        printf("\n");
    }
    // Header gönder
    uint8_t* header_bytes = (uint8_t*)&header;
    for (int i = 0; i < sizeof(PacketHeader); i++) {
        comm.sendByte(header_bytes[i]);
    }
    
    // TX buffer'ın boşalmasını bekle (opsiyonel)
    if (PACKET_WAIT_AFTER_HEADER) {
        comm.waitSendComplete();
    }
    
    if (RFD_ENABLE_PACKET_HEX_LOG) printf("[Packet] Payload bytes: ");
    
    // Payload gönder
    uint8_t payload_buffer[MAX_PACKET_PAYLOAD_SIZE];
    serialize(payload_buffer);
    
    if (RFD_ENABLE_PACKET_HEX_LOG) {
        for (int i = 0; i < header.length; i++) {
            printf("%02X ", payload_buffer[i]);
        }
        printf("\n");
    }

    // Payload'ı küçük parçalar halinde gönder
    const uint32_t chunk_size = PACKET_CHUNK_SIZE;
    for (uint32_t i = 0; i < header.length; i += chunk_size) {
        uint32_t remaining = header.length - i;
        uint32_t current_chunk = (remaining > chunk_size) ? chunk_size : remaining;
        
    // printf("[Packet] Sending chunk %u-%u (%u bytes)\n", i, i + current_chunk - 1, current_chunk);
        
        for (uint32_t j = 0; j < current_chunk; j++) {
            uint32_t byte_index = i + j;
            comm.sendByte(payload_buffer[byte_index]);
            if (PACKET_BYTE_DELAY_US > 0) sleep_us(PACKET_BYTE_DELAY_US);
        }
        
        // Her chunk'tan sonra bekle (opsiyonel)
        if (PACKET_WAIT_EACH_CHUNK) {
            comm.waitSendComplete();
        }
        if (PACKET_CHUNK_DELAY_US > 0) sleep_us(PACKET_CHUNK_DELAY_US);
        
    }

    comm.waitSendComplete();
    printf("[Packet] Sent %s packet (size: %d)\n", 
           packet_type == PACKET_HELLO ? "HELLO" :
           packet_type == PACKET_SIGNED_MESSAGE ? "MESSAGE" :
           packet_type == PACKET_PING ? "PING" :
           packet_type == PACKET_ACK ? "ACK" : 
           packet_type == PACKET_KEY ? "KEY" : "UNKNOWN",
           header.length);
}

std::unique_ptr<Packet> Packet::createFromBuffer(const uint8_t* buffer, uint32_t length, PacketType type) {

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
            auto packet = std::make_unique<AckPacket>(0,0, 0);
            if (packet->deserialize(buffer, length)) {
                return std::move(packet);
            }
            break;
        }
        case PACKET_KEY: {
            auto packet = std::make_unique<KeyPacket>(0, 0, 0, 0);
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
