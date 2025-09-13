#include "RFD.h"
#include "AllPackets.h"
#include <cstring>
#include <cstdio>
#include <pico/stdlib.h>

RFD::RFD(uart_inst_t* uart_inst, uint tx_pin, uint rx_pin) 
    : uart(uart_inst), tx_pin(tx_pin), rx_pin(rx_pin), cryptionService(16) { // 16 bitlik RSA anahtarı oluştur
        initialize();
        initID();
    }

void RFD::initialize() {
    uart_init(uart, 57600);
    gpio_set_function(tx_pin, GPIO_FUNC_UART);
    gpio_set_function(rx_pin, GPIO_FUNC_UART);
    
    printf(" UART initialized on pins TX:%d RX:%d\n", tx_pin, rx_pin);
}

void RFD::initID(){
    // Rastgele ID oluştur (gerçek uygulamada benzersiz ID kullanılması önerilir)
    id = ((uint64_t)rand() << 32) | rand();
    printf("[RFD] Node ID initialized: %llx\n", id);
    rx_buffer_pos = 0;
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

void RFD::processIncomingData(Node& node) {
    PacketHeader header;
    uint8_t payload[256];
    
    if (receivePacket(&header, payload)) {
        printf("[RFD %llx] Received packet type: 0x%02X, length: %d\n", getId(), header.type, header.length);
        
        // Paketi oluştur ve işle
        auto packet = Packet::createFromBuffer(payload, header.length, (PacketType)header.type);
        if (packet) {
            packet->handle(node);
        } else {
            printf("[RFD %llx] Failed to create packet from buffer\n", getId());
        }
    }
}

void RFD::sendHello() {
    auto hello = std::make_unique<HelloPacket>(getId(), 
                                               cryptionService.getPublicKey()->getKey(),
                                               cryptionService.getPublicKey()->getBase());
    printf("[RFD %llx] Sending HELLO packet\n", getId());
    hello->send(uart);
}

void RFD::sendMessage(uint64_t target_id, const char* message) {
    auto msg = std::make_unique<SignedMessagePacket>(getId(), target_id);
    
    uint32_t message_length = strlen(message);

    uint64_t encrypted_data[message_length];

    // cryptionService.Encrypt(message,message_length,encrypted_data);

    // Mesajı payload'a kopyala
    msg->setPayload((const uint8_t*)encrypted_data, message_length);
    
    // Basit imza oluştur (gerçek uygulamada RSA imzası kullanılmalı)
    uint32_t signature[message_length];
    FastMath::myHash(message,signature,message_length);
    // hashi imzala
    msg->setSignature((const uint8_t*)&signature);
    
    printf("[RFD %llx] Sending message to: %llx\n", this->getId(), target_id);
    msg->send(uart);
}

void RFD::sendPing(uint64_t target_id) {
    auto ping = std::make_unique<PingPacket>(this->getId(), target_id, time_us_32());
    printf("[RFD %llx] Sending PING to: %llx\n", this->getId(), target_id);
    ping->send(uart);
}
