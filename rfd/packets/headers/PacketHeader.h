#pragma once
#include <cstdint>
// genel paket
#pragma pack(push, 1) // paddingleri siliyor
struct PacketHeader {
    uint8_t magic1;     // 0xAA
    uint8_t magic2;     // 0x55
    uint8_t type;       // Paket tipi
    uint16_t length;    // Payload uzunluğu
};
#pragma pack(pop)
