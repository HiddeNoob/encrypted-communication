#pragma once
#include <cstdint>

class IComm {
public:
    virtual ~IComm() = default;
    virtual bool isReadable() = 0;
    virtual void sendByte(uint8_t byte) = 0;
    virtual uint8_t readByte() = 0;
    virtual void waitSendComplete() = 0;
};

