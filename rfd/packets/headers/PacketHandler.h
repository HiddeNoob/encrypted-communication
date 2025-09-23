#pragma once
#include "PacketHeader.h"
#include "IComm.h"
#include "rfd_config.h"
#include <cstdint>
#include <memory>

class Packet;

/**
 * PacketHandler - Temiz ve modüler paket okuma işlemleri
 * UART'dan gelen veriyi buffer'da toplar ve complete paketleri çıkarır
 */
class PacketHandler {
private:
    IComm& comm;                                // UART interface
    uint8_t rx_buffer[MAX_RX_BUFFER_SIZE];     // Receive buffer
    size_t rx_buffer_pos;                      // Buffer position
    
    // Debug ve cleanup için timing
    uint32_t last_cleanup_time;
    uint32_t incomplete_packet_start_time;
    size_t last_incomplete_size;
    
    // Buffer yönetimi
    void fillBufferFromComm();
    bool findMagicBytes(size_t& magic_position);
    bool validateHeader(const PacketHeader* header);
    bool isPacketComplete(size_t magic_pos, const PacketHeader* header);
    void extractPacket(size_t magic_pos, const PacketHeader* header, PacketHeader* out_header, uint8_t* out_payload);
    void removeProcessedData(size_t bytes_to_remove);
    
    // Cleanup ve maintenance
    void cleanupOldData();
    void handleIncompletePacket(size_t magic_pos, size_t needed_bytes, size_t available_bytes);
    void periodicBufferMaintenance();

public:
    explicit PacketHandler(IComm& communication);
    ~PacketHandler() = default;
    
    /**
     * Ana paket okuma fonksiyonu
     * @param header Çıkarılan paket header'ı
     * @param payload Çıkarılan paket payload'ı
     * @return true eğer complete paket okunduysa
     */
    bool receivePacket(PacketHeader* header, uint8_t* payload);
    
    /**
     * Buffer durumu bilgisi
     */
    size_t getBufferUsage() const { return rx_buffer_pos; }
    size_t getBufferCapacity() const { return sizeof(rx_buffer); }
    
    /**
     * Buffer'ı manuel olarak temizle
     */
    void clearBuffer();
};