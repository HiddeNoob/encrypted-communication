#include "Packet.h"
#include "RFD.h"

class AckPacket : public Packet {
private:
    uint8_t ack_type;
    uint64_t receiver;

public:
    AckPacket(uint64_t sender,uint64_t receiver, uint8_t type) 
        : Packet(PACKET_ACK, sender), ack_type(type), receiver(receiver) {}

    uint8_t getPayloadSize() const override { return Packet::getPayloadSize() + sizeof(ack_type) + sizeof(receiver); }
    void serialize(uint8_t* buffer) const override;
    bool deserialize(const uint8_t* buffer, uint8_t length) override;
    void handle(Node& node_instance) override;

    uint8_t getAckType() const { return ack_type; }
};
