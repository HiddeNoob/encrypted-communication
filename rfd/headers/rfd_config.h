#pragma once
// Maksimum paket payload boyutu (header hariç)
#define MAX_PACKET_PAYLOAD_SIZE 256

// UART RX buffer boyutu
#define MAX_RX_BUFFER_SIZE 256

// Paket gönderme ayarları
#define PACKET_CHUNK_SIZE 16            // Her defasında gönderilecek byte sayısı
#define PACKET_BYTE_DELAY_US 100        // byte delay
#define PACKET_CHUNK_DELAY_MS 2         // chunk delay 

// Şifrelenmiş mesaj payload boyutu (rsa_data array boyutu)
#define SIGNED_MSG_PAYLOAD_COUNT 32    // 32 * 8 = 128 byte mesaj

// İmza boyutu (rsa_data array boyutu)  
#define SIGNED_MSG_SIGNATURE_COUNT 4    // 4 * 8 = 32 byte imza
