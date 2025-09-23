#include "PacketHandler.h"
#include "AllPackets.h"
#include "pico/time.h"
#include <cstring>
#include <cstdio>

PacketHandler::PacketHandler(IComm& communication) 
    : comm(communication), rx_buffer_pos(0), last_cleanup_time(0), 
      incomplete_packet_start_time(0), last_incomplete_size(0) {
    memset(rx_buffer, 0, sizeof(rx_buffer));
}

bool PacketHandler::receivePacket(PacketHeader* header, uint8_t* payload) {
    // UART'dan yeni veri oku
    fillBufferFromComm();

    // Buffer'da yeterli veri var mı?
    if (rx_buffer_pos < sizeof(PacketHeader)) {
        return false;
    }
    
    // Magic byte'ları ara
    size_t magic_pos;
    if (!findMagicBytes(magic_pos)) {
        // Magic byte bulunamadı, buffer cleanup
        cleanupOldData();
        return false;
    }
    
    // Header'ı oku ve validate et
    PacketHeader temp_header;
    memcpy(&temp_header, &rx_buffer[magic_pos], sizeof(PacketHeader));
    if (!validateHeader(&temp_header)) {
        
        // Geçersiz header, bu magic byte'ı atla
        removeProcessedData(magic_pos + 2); // Magic byte'lardan sonrasına geç
        return false;
    }
    
    // Paket complete mi?
    if (!isPacketComplete(magic_pos, &temp_header)) {
        // Incomplete paket, bekle
        handleIncompletePacket(magic_pos, sizeof(PacketHeader) + temp_header.length, 
                              rx_buffer_pos - magic_pos);
        return false;
    }
    
    // Complete paket bulundu, çıkar
    extractPacket(magic_pos, &temp_header, header, payload);
    
    // Periyodik maintenance
    periodicBufferMaintenance();
    
    return true;
}

void PacketHandler::fillBufferFromComm() {
    bool data_read = false;
    int bytes_read_this_call = 0;
    
    
    while (comm.isReadable() && rx_buffer_pos < sizeof(rx_buffer)) {
        uint8_t byte = comm.readByte();
        rx_buffer[rx_buffer_pos++] = byte;
        data_read = true;
        bytes_read_this_call++;
        // Buffer dolmaya yakınsa uyar
        if (rx_buffer_pos > sizeof(rx_buffer) - 20) {
            printf("[PacketHandler] Buffer warning: %zu/%zu bytes used\n", 
                   rx_buffer_pos, sizeof(rx_buffer));
        }

    }
    if(data_read){
        printf("new data readed: %d bytes\n", bytes_read_this_call);
        printf("[PacketHandler] Buffer position after read: %zu\n", rx_buffer_pos);
        for(int i = rx_buffer_pos - bytes_read_this_call ; i < rx_buffer_pos ; i++){
            printf("%02x ",rx_buffer[i]);
        }
        printf("\n");
    }
    

    // Buffer overflow kontrolü
    if (comm.isReadable() && rx_buffer_pos >= sizeof(rx_buffer)) {
        printf("[PacketHandler] Buffer overflow! Clearing buffer.\n");
        clearBuffer();
    }
}

bool PacketHandler::findMagicBytes(size_t& magic_position) {
    for (int i = (int)rx_buffer_pos - 2; i >= 0; i--) { // stackin en ustundeki magic bytelara baklaım
        if (rx_buffer[i] == 0xAA && rx_buffer[i + 1] == 0x55) {
            magic_position = i;
            printf("[PacketHandler] Magic bytes found at position %d\n", i);
            return true;
        }
    }
    printf("[PacketHandler] No magic bytes found\n");
    return false;
}

bool PacketHandler::validateHeader(const PacketHeader* header) {
    // Magic bytes zaten kontrol edildi
    
    // Length kontrolü
    if (header->length > MAX_PACKET_PAYLOAD_SIZE) {
        printf("[PacketHandler] Invalid packet length: %d (max: %d)\n", 
               header->length, MAX_PACKET_PAYLOAD_SIZE);
        return false;
    }
    
    // Paket tipi kontrolü
    if (header->type < PACKET_HELLO || header->type > PACKET_KEY) {
        printf("[PacketHandler] Unknown packet type: 0x%02X\n", header->type);
        return false;
    }
    
    return true;
}

bool PacketHandler::isPacketComplete(size_t magic_pos, const PacketHeader* header) {
    size_t required_bytes = sizeof(PacketHeader) + header->length;
    size_t available_bytes = rx_buffer_pos - magic_pos;
    
    printf("[PacketHandler] Checking completion: magic_pos=%zu, required=%zu, available=%zu, buffer_pos=%zu\n",
           magic_pos, required_bytes, available_bytes, rx_buffer_pos);
    
    printf("[PacketHandler] Complete buffer (%zu bytes): ", rx_buffer_pos);
    for (size_t i = 0; i < rx_buffer_pos; i++) {
        printf("%02X ", rx_buffer[i]);
    }
    printf("\n");
    
    bool is_complete = available_bytes >= required_bytes;
    printf("[PacketHandler] Packet complete: %s\n", is_complete ? "YES" : "NO");
    
    return is_complete;
}

void PacketHandler::extractPacket(size_t magic_pos, const PacketHeader* header, 
                                 PacketHeader* out_header, uint8_t* out_payload) {
    // Header'ı kopyala
    memcpy(out_header, header, sizeof(PacketHeader));
    
    // Payload'ı kopyala
    memcpy(out_payload, &rx_buffer[magic_pos + sizeof(PacketHeader)], header->length);
    
    printf("[PacketHandler] Packet extracted - Type: 0x%02X, Length: %d\n", 
           header->type, header->length);
    
    printf("[PacketHandler] Raw packet data: ");
    for (size_t i = magic_pos; i < magic_pos + sizeof(PacketHeader) + header->length; i++) {
        printf("%02X ", rx_buffer[i]);
    }
    printf("\n");
    
    // İşlenen veriyi buffer'dan kaldır
    size_t total_packet_size = sizeof(PacketHeader) + header->length;
    removeProcessedData(magic_pos + total_packet_size);
}

void PacketHandler::removeProcessedData(size_t bytes_to_remove) {
    printf("[PacketHandler] removeProcessedData: removing %zu bytes from buffer (current size: %zu)\n", 
           bytes_to_remove, rx_buffer_pos);
    
    if (bytes_to_remove >= rx_buffer_pos) {
        // Tüm buffer temizleniyor
        printf("[PacketHandler] Clearing entire buffer\n");
        rx_buffer_pos = 0;
        memset(rx_buffer, 0, sizeof(rx_buffer)); // Buffer'ı sıfırla
    } else { 
        // Kalan veriyi buffer başına taşı
        size_t remaining = rx_buffer_pos - bytes_to_remove;
        printf("[PacketHandler] Moving %zu remaining bytes to start\n", remaining);
        
        // Before move - show what's being removed and what's staying
        printf("[PacketHandler] Removing bytes: ");
        for (size_t i = 0; i < bytes_to_remove && i < rx_buffer_pos; i++) {
            printf("%02X ", rx_buffer[i]);
        }
        printf("\n");
        
        printf("[PacketHandler] Keeping bytes: ");
        for (size_t i = bytes_to_remove; i < rx_buffer_pos; i++) {
            printf("%02X ", rx_buffer[i]);
        }
        printf("\n");
        
        memmove(rx_buffer, &rx_buffer[bytes_to_remove], remaining);
        rx_buffer_pos = remaining;
        
        // Clear the rest of the buffer to avoid confusion
        memset(&rx_buffer[rx_buffer_pos], 0, sizeof(rx_buffer) - rx_buffer_pos);
        
        printf("[PacketHandler] After cleanup, buffer size: %zu\n", rx_buffer_pos);
    }
}

void PacketHandler::handleIncompletePacket(size_t magic_pos, size_t needed_bytes, size_t available_bytes) {
    uint32_t current_time = time_us_32();
    
    printf("[PacketHandler] handleIncompletePacket: magic_pos=%zu, needed=%zu, available=%zu\n",
           magic_pos, needed_bytes, available_bytes);
    
    // Magic byte'tan önceki garbage'ı temizle
    if (magic_pos > 0) {
        removeProcessedData(magic_pos);
        printf("[PacketHandler] Cleaned %zu garbage bytes before magic\n", magic_pos);
        return; // Return early after cleanup to avoid timeout logic
    }
    
    // Timeout kontrolü
    if (last_incomplete_size != rx_buffer_pos) {
        // Yeni incomplete paket
        incomplete_packet_start_time = current_time;
        last_incomplete_size = rx_buffer_pos;
        printf("[PacketHandler] Starting incomplete packet timer\n");
    } else if (current_time - incomplete_packet_start_time > 2000000) { // 2 saniye
        printf("[PacketHandler] Incomplete packet timeout, clearing buffer\n");
        clearBuffer();
        return;
    }
    
}

void PacketHandler::cleanupOldData() {
    // Magic byte bulunamadıysa ve buffer doluysa, buffer'ı temizle
    if (rx_buffer_pos >= sizeof(rx_buffer) - 10) {
        printf("[PacketHandler] No magic bytes found, clearing buffer (%zu bytes)\n", rx_buffer_pos);
        clearBuffer();
    }
}

void PacketHandler::periodicBufferMaintenance() {
    uint32_t current_time = time_us_32();
    
    // 1 saniyede bir maintenance
    if (current_time - last_cleanup_time > 1000000) {
        if (rx_buffer_pos > 20) { // Boş buffer'lar için log yazdırma
            printf("[PacketHandler] Periodic maintenance - %zu bytes in buffer\n", rx_buffer_pos);
            
            // Magic byte arayan cleanup
            size_t magic_pos;
            if (!findMagicBytes(magic_pos)) {
                printf("[PacketHandler] No valid magic bytes, clearing buffer\n");
                clearBuffer();
            } else if (magic_pos > 0) {
                printf("[PacketHandler] Cleaning %zu garbage bytes\n", magic_pos);
                removeProcessedData(magic_pos);
            }
        }
        last_cleanup_time = current_time;
    }
}

void PacketHandler::clearBuffer() {
    rx_buffer_pos = 0;
    last_incomplete_size = 0;
    incomplete_packet_start_time = 0;
}