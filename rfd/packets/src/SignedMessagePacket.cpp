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

bool SignedMessagePacket::deserialize(const uint8_t* buffer, uint16_t length) {
    if (length < getPayloadSize()) return false;

    Packet::deserialize(buffer, length);
    size_t offset = Packet::getPayloadSize();
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
        printf("[SignedMessagePacket] Message for me!");

        rsa_data* data = node_instance.getRFD()
                                .getCryptionService()
                                .getPrivateKey()
                                ->sign(
                                    (void*)payload,
                                    sizeof(payload) / sizeof(payload[0]),
                                    sizeof(rsa_data)
                                );

        printf("\nDecrypted data: ");
        for (size_t i = 0; ((char*)(data[i])) ; i++) {
            printf("%c", ((char*)(data[i]))); // sifrelenen veri char tipindeydi
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
void SignedMessagePacket::setPayload(const void* data, size_t len) {
    if(len > sizeof(payload)) {
        printf("[SignedMessagePacket] Warning: Payload length %zu exceeds buffer %zu, truncating.\n", len, sizeof(payload));
        len = sizeof(payload);
    }
    memset(payload, 0, sizeof(payload));
    memcpy(payload, data, len);
}

void SignedMessagePacket::setSignature(const void* sig, size_t len) {
    if (len > sizeof(signature)) {
        printf("[SignedMessagePacket] Warning: Signature length %zu exceeds buffer %zu, truncating.\n", len, sizeof(signature));
        len = sizeof(signature);
    }
    memset(signature, 0, sizeof(signature));
    memcpy(signature, sig, len);
}
