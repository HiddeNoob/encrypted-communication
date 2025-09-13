#include "SignedMessagePacket.h"
#include "AckPacket.h"


// SignedMessagePacket
void SignedMessagePacket::serialize(uint8_t* buffer) const {
    Packet::serialize(buffer);
    size_t offset = Packet::getPayloadSize();
    memcpy(buffer + offset, &target_id, sizeof(target_id));
    offset += sizeof(target_id);
    memcpy(buffer + offset, payload, sizeof(payload));
    offset += sizeof(payload);
    memcpy(buffer + offset, signature, sizeof(signature));
}

bool SignedMessagePacket::deserialize(const uint8_t* buffer, uint8_t length) {
    if (length < getPayloadSize()) return false;

    Packet::deserialize(buffer, length);
    size_t offset = Packet::getPayloadSize();
    memcpy(&sender_id, buffer + offset, sizeof(sender_id));
    offset += sizeof(sender_id);
    memcpy(&target_id, buffer + offset, sizeof(target_id));
    offset += sizeof(target_id);
    memcpy(payload, buffer + offset, sizeof(payload));
    offset += sizeof(payload);
    memcpy(signature, buffer + offset, sizeof(signature));
    return true;
}

void SignedMessagePacket::handle(Node& node_instance) {
    printf("[SignedMessagePacket] Received message from: %llx to: %llx\n", 
           sender_id, target_id);

    if (target_id == node_instance.getId()) {
        printf("[SignedMessagePacket] Message for me! Payload: ");
        for (int i = 0; i < 32 && payload[i] != 0; i++) {
            printf("%c", payload[i]);
        }
        printf("\n");
        
        // ACK gönder
        auto ack = std::make_unique<AckPacket>(node_instance.getId(), sender_id, PACKET_SIGNED_MESSAGE);
        ack->send(node_instance.getRFD().getUart());
    } else {
        printf("[SignedMessagePacket] Message not for me, ignoring\n");
    }
}

// 
void SignedMessagePacket::setPayload(const uint8_t* data, size_t len) {
    if(len > 32) {
        printf("[SignedMessagePacket] Warning: Payload length exceeds 32 bytes, truncating.\n");
        len = 32; // maksimum 32 byte
    }
    memset(payload, 0, 32);
    memcpy(payload, data, len);
}

void SignedMessagePacket::setSignature(const uint8_t* sig) {
    memcpy(signature, sig, 8);
}
