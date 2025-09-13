#include "Packet.h"
#include "RFD.h"


// HELLO Paketi, ağa ilk bağlanıldığında diğerleri ile 
// şifreli haberleşebilmek için public keyini paylaşır
class HelloPacket : public Packet {
private:
    uint64_t public_key;
    uint64_t modulus;

public:
    HelloPacket(uint64_t sender, uint64_t pub_key, uint64_t mod) 
        : Packet(PACKET_HELLO, sender), public_key(pub_key), modulus(mod) {}

    uint8_t getPayloadSize() const override { return Packet::getPayloadSize() + sizeof(public_key) + sizeof(modulus); }
    void serialize(uint8_t* buffer) const override;
    bool deserialize(const uint8_t* buffer, uint8_t length) override;
    void handle(Node& node_instance) override;

    // Getters
    uint64_t getPublicKey() const { return public_key; }
    uint64_t getModulus() const { return modulus; }
};