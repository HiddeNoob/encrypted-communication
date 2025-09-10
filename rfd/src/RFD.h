#pragma once
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "RSA.h"
#include "Packet.h"
#include <unordered_map>
#include <memory>

class RFD {
private:
    RSA cryptionService;
    uint64_t my_id;
    std::unordered_map<uint64_t, uint64_t> networkPublicKeys; // <node_id, public_key>
    std::unordered_map<uint64_t, uint64_t> networkModulus;    // <node_id, modulus>
    
    uart_inst_t* uart;
    uint tx_pin, rx_pin;
    
    uint8_t rx_buffer[256];
    size_t rx_buffer_pos;
    
    void initID();
    bool receivePacket(PacketHeader* header, uint8_t* payload);

public:
    RFD(uart_inst_t* uart_inst, uint tx_pin, uint rx_pin);
    
    // Ana fonksiyonlar
    void initialize();
    void processIncomingData();
    
    void sendHello(); // ağı tara
    void sendMessage(uint64_t target_id, const char* message); // ağa mesaj gönder
    void sendPing(uint64_t target_id); // ağdan bir node'a ping gönder
    
    // ağa katılan kişiyi kaydet
    void registerNode(uint64_t node_id, uint64_t public_key, uint64_t modulus);
    
    // Getter fonksiyonlar
    uint64_t getMyId() const { return my_id; }
    size_t getKnownNodesCount() const { return networkPublicKeys.size(); }
    uint64_t getFirstKnownNodeId() const { 
        if (networkPublicKeys.empty()) return 0;
        return networkPublicKeys.begin()->first;
    }
    uart_inst_t* getUart() const { return uart; }
    RSA& getCryptionService() { return cryptionService; }
};