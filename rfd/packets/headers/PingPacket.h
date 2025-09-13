#include "Packet.h"
#include "RFD.h"

class PingPacket : public Packet {
private:
    uint64_t target_id;
    uint32_t timestamp;

public:
    PingPacket(uint64_t sender, uint64_t target, uint32_t ts) 
        : Packet(PACKET_PING, sender), target_id(target), timestamp(ts) {}

    uint8_t getPayloadSize() const override { 
        return Packet::getPayloadSize() + sizeof(target_id) + sizeof(timestamp); 
    }
    void serialize(uint8_t* buffer) const override;
    bool deserialize(const uint8_t* buffer, uint8_t length) override;
    void handle(Node& node_instance) override;

    uint64_t getTargetId() const { return target_id; }
    uint32_t getTimestamp() const { return timestamp; }
};
