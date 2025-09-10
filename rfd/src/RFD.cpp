#include "RFD.h"
#include <cstring>
#include <cstdio>

RFD::RFD(uart_inst_t* uart_inst, uint tx_pin, uint rx_pin) 
    : cryptionService(16), uart(uart_inst), tx_pin(tx_pin), rx_pin(rx_pin), rx_buffer_pos(0) {
    initID();
}

void RFD::initialize() {
    uart_init(uart, 57600);
    gpio_set_function(tx_pin, GPIO_FUNC_UART);
    gpio_set_function(rx_pin, GPIO_FUNC_UART);
    
    printf("[RFD %llx] UART initialized on pins TX:%d RX:%d\n", my_id, tx_pin, rx_pin);
    printf("[RFD %llx] My ID: %llx\n", my_id, my_id);
}

void RFD::initID() {
    // Basit ID üretimi
    my_id = (uint64_t)time_us_64() ^ (uint64_t)0xDEADBEEF;
    printf("[RFD %llx] Generated ID: %llx\n", my_id, my_id);
}

bool RFD::receivePacket(PacketHeader* header, uint8_t* payload) {
    // UART'dan veri oku
    while (uart_is_readable(uart)) {
        uint8_t byte = uart_getc(uart);
        
        if (rx_buffer_pos < sizeof(rx_buffer)) {
            rx_buffer[rx_buffer_pos++] = byte;
        }
    }
    
    // TODO packet okumada beklenmeyen durumlar oluyor düzeltilecek

    // Yeterli veri var mı kontrol et
    if (rx_buffer_pos < sizeof(PacketHeader)) {
        return false;
    }
    
    // Magic byte'ları ara
    for (size_t i = 0; i <= rx_buffer_pos - sizeof(PacketHeader); i++) {
        if (rx_buffer[i] == 0xAA && rx_buffer[i + 1] == 0x55) {
            // Header'ı kopyala
            memcpy(header, &rx_buffer[i], sizeof(PacketHeader));
            
            // Toplam paket uzunluğunu kontrol et
            size_t total_packet_size = sizeof(PacketHeader) + header->length;
            if (i + total_packet_size <= rx_buffer_pos) {
                // Payload'ı kopyala
                memcpy(payload, &rx_buffer[i + sizeof(PacketHeader)], header->length);
                
                // Buffer'ı temizle (kullanılan kısmı kaldır)
                size_t remaining = rx_buffer_pos - (i + total_packet_size);
                memmove(rx_buffer, &rx_buffer[i + total_packet_size], remaining);
                rx_buffer_pos = remaining;
                
                return true;
            }
        }
    }
    
    return false;
}

void RFD::processIncomingData() {
    PacketHeader header;
    uint8_t payload[256];
    
    if (receivePacket(&header, payload)) {
        printf("[RFD %llx] Received packet type: 0x%02X, length: %d\n", my_id, header.type, header.length);
        
        // Paketi oluştur ve işle
        auto packet = Packet::createFromBuffer(payload, header.length, (PacketType)header.type);
        if (packet) {
            packet->handle(this);
        } else {
            printf("[RFD %llx] Failed to create packet from buffer\n", my_id);
        }
    }
}

void RFD::sendHello() {
    auto hello = std::make_unique<HelloPacket>(my_id, 
                                               cryptionService.getPublicKey(), 
                                               cryptionService.getModulus());
    printf("[RFD %llx] Sending HELLO packet\n", my_id);
    hello->send(uart);
}

void RFD::sendMessage(uint64_t target_id, const char* message) {
    auto msg = std::make_unique<SignedMessagePacket>(my_id, target_id);
    
    // Mesajı payload'a kopyala
    msg->setPayload((const uint8_t*)message, strlen(message));
    
    // Basit imza oluştur (gerçek uygulamada RSA imzası kullanılmalı)
    uint64_t simple_signature = my_id ^ target_id;
    msg->setSignature((const uint8_t*)&simple_signature);
    
    printf("[RFD %llx] Sending message to: %llx\n", my_id, target_id);
    msg->send(uart);
}

void RFD::sendPing(uint64_t target_id) {
    auto ping = std::make_unique<PingPacket>(my_id, target_id, time_us_32());
    printf("[RFD %llx] Sending PING to: %llx\n", my_id, target_id);
    ping->send(uart);
}

void RFD::registerNode(uint64_t node_id, uint64_t public_key, uint64_t modulus) {
    networkPublicKeys[node_id] = public_key;
    networkModulus[node_id] = modulus;
    printf("[RFD %llx] Registered node: %llx with public key: %llx\n", my_id, node_id, public_key);
}
