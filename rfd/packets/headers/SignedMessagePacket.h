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
    rsa_data signature;   // Single signature for message hash

public:
    SignedMessagePacket(uint64_t sender, uint64_t target) 
        : Packet(PACKET_SIGNED_MESSAGE, sender), target_id(target), signature(0) {
        memset(payload, 0, sizeof(payload));
    }

    uint16_t getPayloadSize() const override {
        return Packet::getPayloadSize() +
               sizeof(target_id) + sizeof(payload) + sizeof(signature);
    }
    void serialize(uint8_t* buffer) const override;
    bool deserialize(const uint8_t* buffer, uint16_t length) override;
    void handle(Node& node_instance) override;

    void setPayload(const void* data, size_t len);
    void setSignature(rsa_data sig);
    uint64_t getTargetId() const { return target_id; }
    const rsa_data* getPayload() const { return payload; }
    rsa_data getSignature() const { return signature; }
    
    bool checkSignature(const uint8_t* decryptedMessage,const RemoteNode* senderNode); 
};

