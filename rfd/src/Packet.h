#pragma once
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <memory>
#include <cstring>

class RFD;

// Paket tipleri
enum PacketType {
    PACKET_HELLO = 0x01,
    PACKET_SIGNED_MESSAGE = 0x02,
    PACKET_PING = 0x03,
    PACKET_ACK = 0x04,
    PACKET_ERROR = 0x05
};

// genel paket
#pragma pack(push, 1) // paddingleri siliyor
struct PacketHeader {
    uint8_t magic1;     // 0xAA
    uint8_t magic2;     // 0x55
    uint8_t type;       // Paket tipi
    uint8_t length;     // Payload uzunluğu
};
#pragma pack(pop)

// Soyut paket
class Packet {
protected:
    PacketType packet_type;
    uint64_t sender_id;

public:
    Packet(PacketType type, uint64_t sender) : packet_type(type), sender_id(sender) {}
    virtual ~Packet() = default;

    // soyut fonksiyonlar
    virtual uint8_t getPayloadSize() const = 0;
    virtual void serialize(uint8_t* buffer) const = 0;
    virtual bool deserialize(const uint8_t* buffer, uint8_t length) = 0;
    virtual void handle(RFD* rfd_instance) = 0;

    PacketType getType() const { return packet_type; }
    uint64_t getSenderId() const { return sender_id; }

    // buffer'dan paket oluşturur
    static std::unique_ptr<Packet> createFromBuffer(const uint8_t* buffer, uint8_t length, PacketType type);

    void send(uart_inst_t* uart) const;
};


// HELLO Paketi, ağa ilk bağlanıldığında diğerleri ile 
// şifreli haberleşebilmek için public keyini paylaşır
class HelloPacket : public Packet {
private:
    uint64_t public_key;
    uint64_t modulus;

public:
    HelloPacket(uint64_t sender, uint64_t pub_key, uint64_t mod) 
        : Packet(PACKET_HELLO, sender), public_key(pub_key), modulus(mod) {}

    uint8_t getPayloadSize() const override { return sizeof(uint64_t) * 3; }
    void serialize(uint8_t* buffer) const override;
    bool deserialize(const uint8_t* buffer, uint8_t length) override;
    void handle(RFD* rfd_instance) override;

    // Getters
    uint64_t getPublicKey() const { return public_key; }
    uint64_t getModulus() const { return modulus; }
};

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

    uint8_t getPayloadSize() const override { return sizeof(uint64_t) * 2 + 32 + 8; }
    void serialize(uint8_t* buffer) const override;
    bool deserialize(const uint8_t* buffer, uint8_t length) override;
    void handle(RFD* rfd_instance) override;

    void setPayload(const uint8_t* data, size_t len);
    void setSignature(const uint8_t* sig);
    uint64_t getTargetId() const { return target_id; }
    const uint8_t* getPayload() const { return payload; }
    const uint8_t* getSignature() const { return signature; }
};

class PingPacket : public Packet {
private:
    uint64_t target_id;
    uint32_t timestamp;

public:
    PingPacket(uint64_t sender, uint64_t target, uint32_t ts) 
        : Packet(PACKET_PING, sender), target_id(target), timestamp(ts) {}

    uint8_t getPayloadSize() const override { return sizeof(uint64_t) * 2 + sizeof(uint32_t); }
    void serialize(uint8_t* buffer) const override;
    bool deserialize(const uint8_t* buffer, uint8_t length) override;
    void handle(RFD* rfd_instance) override;

    uint64_t getTargetId() const { return target_id; }
    uint32_t getTimestamp() const { return timestamp; }
};

class AckPacket : public Packet {
private:
    uint8_t ack_type;

public:
    AckPacket(uint64_t sender, uint8_t type) 
        : Packet(PACKET_ACK, sender), ack_type(type) {}

    uint8_t getPayloadSize() const override { return sizeof(uint64_t) + sizeof(uint8_t); }
    void serialize(uint8_t* buffer) const override;
    bool deserialize(const uint8_t* buffer, uint8_t length) override;
    void handle(RFD* rfd_instance) override;

    uint8_t getAckType() const { return ack_type; }
};
