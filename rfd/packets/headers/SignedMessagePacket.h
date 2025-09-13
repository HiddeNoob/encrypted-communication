#include "Packet.h"
#include <cstring>
#include "RFD.h"

// Şifrelenmiş Mesaj Paketi
class SignedMessagePacket : public Packet {
private:
    uint64_t target_id;
    uint8_t payload[32];
    uint8_t signature[8];

public:
    SignedMessagePacket(uint64_t sender, uint64_t target) 
        : Packet(PACKET_SIGNED_MESSAGE, sender), target_id(target) {
        memset(payload, 0, 32);
        memset(signature, 0, 8);
    }

    uint8_t getPayloadSize() const override {
        return Packet::getPayloadSize() +
               sizeof(target_id) + sizeof(payload) + sizeof(signature);
    }
    void serialize(uint8_t* buffer) const override;
    bool deserialize(const uint8_t* buffer, uint8_t length) override;
    void handle(Node& node_instance) override;

    void setPayload(const uint8_t* data, size_t len);
    void setSignature(const uint8_t* sig);
    uint64_t getTargetId() const { return target_id; }
    const uint8_t* getPayload() const { return payload; }
    const uint8_t* getSignature() const { return signature; }
};

