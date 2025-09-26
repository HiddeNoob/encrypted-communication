#include "Packet.h"
#include "Node.h"

class KeyPacket : public Packet {
private:
    uint64_t target_id;
    uint64_t public_key;
    uint64_t modulus;

public:
    KeyPacket(uint64_t sender, uint64_t target, uint64_t pub_key, uint64_t mod)
        : Packet(PACKET_KEY, sender), target_id(target), public_key(pub_key), modulus(mod) {}

    uint16_t getPayloadSize() const override { 
        return Packet::getPayloadSize() + sizeof(target_id) + sizeof(public_key) + sizeof(modulus);
    }
    void serialize(uint8_t* buffer) const override;
    bool deserialize(const uint8_t* buffer, uint16_t length) override;
    void handle(Node& node_instance) override;

    uint64_t getTargetId() const { return target_id; }
};
