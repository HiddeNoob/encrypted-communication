#pragma once
#include "hardware/uart.h"
#include "RSA.h"
#include "UART.h"
#include "PacketHeader.h"
#include "PacketHandler.h"
#include "rfd_config.h"
#include <unordered_map>
#include <memory>

class Node;

class RFD {
private:
    RSA cryptionService;
    uint64_t id;
    UART uart;
    std::unique_ptr<PacketHandler> packet_handler;
    
    void initID();

public:
    RFD(uart_inst_t* uart_inst, uint tx_pin, uint rx_pin);

    // Ana fonksiyonlar
    void processIncomingData(Node& node);
    void sendHello();
    void sendPing(uint64_t target_id);
    void sendMessage(uint64_t target_id, const char* message);
    
    Node* connectedNode;
    
    // Getter fonksiyonlar
    RSA& getCryptionService() { return cryptionService; }
    uint64_t getId() const { return id; }
    UART& getUart() { return uart; }
};