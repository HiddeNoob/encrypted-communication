#include "AllPackets.h"

void Packet::serialize(uint8_t* buffer) const {
    memcpy(buffer, &sender_id, sizeof(sender_id));
}

bool Packet::deserialize(const uint8_t* buffer, uint8_t length) {
    if (length < getPayloadSize()) return false;

    memcpy(&sender_id, buffer, sizeof(sender_id));
    return true;
}


// Ana Packet sınıfı metodları
void Packet::send(uart_inst_t* uart) const {
    PacketHeader header;
    header.magic1 = 0xAA;
    header.magic2 = 0x55;
    header.type = (uint8_t)packet_type;
    header.length = getPayloadSize();

    printf("[Packet] Preparing to send packet type: 0x%02X, length: %d\n", header.type, header.length);

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
    printf("Create from buffer: type=0x%02X, length=%d\n", type, length);
    for (size_t i = 0; i < length; i++)
    {
        printf("%02X ", buffer[i]);
    }
    printf("\n");

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
        default:
            printf("[Packet] Unknown packet type: 0x%02X\n", type);
            break;
    }
    return nullptr;
}
