#pragma once
#include "hardware/uart.h"
#include "RSA.h"
#include "PacketHeader.h"
#include <unordered_map>
class Node;

class RFD {
private:
    RSA cryptionService;
    uint64_t id;

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
    void processIncomingData(Node& node);
    
    void sendHello(); // ağı tara
    void sendMessage(uint64_t target_id, const char* message); // ağa mesaj gönder
    void sendPing(uint64_t target_id); // ağdan bir node'a ping gönder
    
    Node* connectedNode;
    
    // Getter fonksiyonlar
    uint64_t getId() const { return id; }
    uart_inst_t* getUart() const { return uart; }
};