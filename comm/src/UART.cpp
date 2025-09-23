#include "UART.h"

UART::UART(uart_inst_t* uart_instance, uint tx_pin, uint rx_pin, uint baudrate)
    : uart(uart_instance), tx_pin(tx_pin), rx_pin(rx_pin), baudrate(baudrate) {
    uart_init(uart, baudrate);
    gpio_set_function(tx_pin, GPIO_FUNC_UART);
    gpio_set_function(rx_pin, GPIO_FUNC_UART);
    
    printf("UART initialized on pins TX:%d RX:%d at %d baud\n", tx_pin, rx_pin, baudrate);
}

bool UART::isReadable() {
    return uart_is_readable(uart);
}

void UART::sendByte(uint8_t byte) {
    uart_putc_raw(uart, byte);
}

uint8_t UART::readByte() {
    return uart_getc(uart);
}

void UART::waitSendComplete() {
    uart_tx_wait_blocking(uart);
}
