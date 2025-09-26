#include "Packet.h"
#include "RFD.h"

class PingPacket : public Packet {
private:
    uint64_t target_id;

public:
    PingPacket(uint64_t sender, uint64_t target, uint32_t ts) 
        : Packet(PACKET_PING, sender), target_id(target) {}

    uint16_t getPayloadSize() const override { 
        return Packet::getPayloadSize() + sizeof(target_id); 
    }
    void serialize(uint8_t* buffer) const override;
    bool deserialize(const uint8_t* buffer, uint16_t length) override;
    void handle(Node& node_instance) override;

    uint64_t getTargetId() const { return target_id; }
};
