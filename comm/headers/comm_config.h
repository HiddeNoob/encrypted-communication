#pragma once

// ====== UART KONFİGÜRASYONLARI ======

// Varsayılan UART ayarları
#define DEFAULT_UART_BAUDRATE 57600     // Varsayılan baud rate
#define UART_TX_BUFFER_SIZE 256         // TX buffer boyutu
#define UART_RX_BUFFER_SIZE 256         // RX buffer boyutu

// UART timeout ayarları
#define UART_READ_TIMEOUT_US 1000       // Okuma timeout (mikrosaniye)
#define UART_WRITE_TIMEOUT_US 10000     // Yazma timeout (mikrosaniye)