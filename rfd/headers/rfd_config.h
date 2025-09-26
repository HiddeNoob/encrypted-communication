#pragma once
#include <PacketHeader.h>
// Maksimum paket payload boyutu (header hariç)
#define MAX_PACKET_PAYLOAD_SIZE 512

// UART RX buffer boyutu
#define MAX_RX_BUFFER_SIZE (MAX_PACKET_PAYLOAD_SIZE + sizeof(PacketHeader))

// Paket gönderme ayarları
#define PACKET_CHUNK_SIZE 512            // Her defasında gönderilecek byte sayısı
#define PACKET_BYTE_DELAY_US 0          // byte delay
#define PACKET_CHUNK_DELAY_US 0         // chunk delay

#define PACKET_WAIT_AFTER_HEADER 0      // Header sonrası waitSendComplete
#define PACKET_WAIT_EACH_CHUNK 0        // Her chunk sonrası waitSendComplete

#define PACKET_TIME_OUT_MS 200 // bu süre kadar veri gelmez ise tamamlanmamış paket silinir.

// Şifrelenmiş mesaj payload boyutu (rsa_data array boyutu)
#define SIGNED_MSG_PAYLOAD_COUNT 32    // 32 * 8 = 256 byte mesaj

// İmza boyutu (rsa_data array boyutu)  
#define SIGNED_MSG_SIGNATURE_COUNT 4    // 4 * 8 = 32 byte imza

#define RFD_ENABLE_PACKET_HEX_LOG 0     // Header/Payload hex dump
#define RFD_ENABLE_RX_VERBOSE 0         // PacketHandler ayrıntılı loglar
