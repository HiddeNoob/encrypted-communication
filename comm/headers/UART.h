#pragma once
#include "IComm.h"
#include <pico/stdlib.h>
#include <hardware/uart.h>
#include <cstdio>

class UART : public IComm {
public:
    UART(uart_inst_t* uart_instance, uint tx_pin, uint rx_pin, uint baudrate = 57600);

    bool isReadable() override;

    void sendByte(uint8_t byte) override;

    uint8_t readByte() override;

    void waitSendComplete() override;

private:
    uart_inst_t* uart;
    uint tx_pin, rx_pin;
    uint baudrate;
};