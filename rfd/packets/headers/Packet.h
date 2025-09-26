#pragma once
#include <memory>
#include "RFD.h"
#include <Node.h>

// Paket tipleri
enum PacketType {
    PACKET_HELLO = 0x01,
    PACKET_SIGNED_MESSAGE = 0x02,
    PACKET_PING = 0x03,
    PACKET_ACK = 0x04,
    PACKET_KEY = 0x05,
    PACKET_ERROR = 0x06
};

// Soyut paket
class Packet {
protected:
    PacketType packet_type;
    uint64_t sender_id;

public:
    Packet(PacketType type, uint64_t sender) : packet_type(type), sender_id(sender) {}
    virtual ~Packet() = default;

    // soyut fonksiyonlar
    virtual uint16_t getPayloadSize() const { return sizeof(uint64_t); };
    virtual void serialize(uint8_t* buffer) const;
    virtual bool deserialize(const uint8_t* buffer, uint16_t length);
    virtual void handle(Node& node_instance) = 0;

    PacketType getType() const { return packet_type; }
    uint64_t getSenderId() const { return sender_id; }

    // buffer'dan paket oluşturur
    static std::unique_ptr<Packet> createFromBuffer(const uint8_t* buffer, uint32_t length, PacketType type);  // uint32_t yap

    void send(IComm& comm) const;
};