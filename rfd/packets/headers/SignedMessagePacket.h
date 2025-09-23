#include "Packet.h"
#include <cstring>
#include "RFD.h"
#include "rfd_config.h"
#include "config.h"  // RSA config için

// Şifrelenmiş Mesaj Paketi
class SignedMessagePacket : public Packet {
private:
    uint64_t target_id;
    rsa_data payload[SIGNED_MSG_PAYLOAD_COUNT];    // Config'den al
    rsa_data signature[SIGNED_MSG_SIGNATURE_COUNT];   // Config'den al

public:
    SignedMessagePacket(uint64_t sender, uint64_t target) 
        : Packet(PACKET_SIGNED_MESSAGE, sender), target_id(target) {
        memset(payload, 0, sizeof(payload));
        memset(signature, 0, sizeof(signature));
    }

    uint8_t getPayloadSize() const override {
        return Packet::getPayloadSize() +
               sizeof(target_id) + sizeof(payload) + sizeof(signature);
    }
    void serialize(uint8_t* buffer) const override;
    bool deserialize(const uint8_t* buffer, uint8_t length) override;
    void handle(Node& node_instance) override;

    void setPayload(const void* data, size_t len);
    void setSignature(const void* sig);
    uint64_t getTargetId() const { return target_id; }
    const rsa_data* getPayload() const { return payload; }
    const rsa_data* getSignature() const { return signature; }
};

