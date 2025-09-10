#include <iostream>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/mutex.h"
#include <cstring>
#include "RSA.h"
#include "RFD.h"

mutex_t printf_mutex;
volatile bool core1_ready = false;
char usb_command_buffer[256];
volatile bool new_command = false;

void parse_and_execute_command(const char* cmd, RFD& node1) {
    printf("\n[USB] komut alindi: %s\n", cmd);
    
    if (strncmp(cmd, "n1:", 3) == 0) {
        // node1 komutlari
        if (strncmp(cmd + 3, "hello", 5) == 0) {
            printf("[USB] node1 hello paketi gonderiliyor...\n");
            node1.sendHello();
        }
        else if (strncmp(cmd + 3, "ping:", 5) == 0) {
            uint64_t target_id = strtoull(cmd + 8, NULL, 16);
            printf("[USB] node1 ping gonderiliyor -> %llx\n", target_id);
            node1.sendPing(target_id);
        }
        else if (strncmp(cmd + 3, "msg:", 4) == 0) {
            // format: n1:msg:hedef_id:mesaj
            char* colon1 = strchr(cmd + 7, ':');
            if (colon1) {
                *colon1 = '\0';
                uint64_t target_id = strtoull(cmd + 7, NULL, 16);
                char* message = colon1 + 1;
                printf("[USB] node1 mesaj gonderiliyor -> %llx: %s\n", target_id, message);
                node1.sendMessage(target_id, message);
                *colon1 = ':'; // geri koy
            }
        }
    }
    else if (strncmp(cmd, "n2:", 3) == 0) {
        // node 2 yonetimi suanlik yok
    }
    else if (strcmp(cmd, "help") == 0) {
        printf("=== komut listesi ===\n");
        printf("n1:hello - node1 hello paketi gonder\n");
        printf("n1:ping:hedef_id - node1 ping gonder (hex id)\n");
        printf("n1:msg:hedef_id:mesaj - node1 mesaj gonder\n");
        printf("help - bu yardim mesajini goster\n");
        printf("ornek: n1:hello\n");
        printf("ornek: n1:ping:1234567890abcdef\n");
        printf("ornek: n1:msg:1234567890abcdef:selam nasılsin\n");
    }
    else {
        printf("[USB] bilinmeyen komut: %s (help yazin)\n", cmd);
    }
}

void core1_main() {
    RFD rfd_node2(uart1, 4, 5);
    rfd_node2.initialize();
    
    core1_ready = true;
    
    mutex_enter_blocking(&printf_mutex);
    printf("[CORE1] node2 hazir, id: %llx\n", rfd_node2.getMyId());
    mutex_exit(&printf_mutex);
    
    while (true) {
        rfd_node2.processIncomingData();
        
        sleep_ms(10);
    }
}

int main() {
    stdio_init_all();
    
    // mutex'i baslat
    mutex_init(&printf_mutex);
    
    printf("=== rfd manual komut sistemi ===\n");
    printf("node1: uart0 (gpio 0-1)\n");
    printf("node2: uart1 (gpio 4-5)\n");
    printf("komutlar icin 'help' yazin\n");
    sleep_ms(2000);
    
    multicore_launch_core1(core1_main);
    
    sleep_ms(100);
    RFD rfd_node1(uart0, 0, 1);
    rfd_node1.initialize();
    
    printf("[CORE0] node1 hazir, id: %llx\n", rfd_node1.getMyId());
    
    while (!core1_ready) {
        sleep_ms(10);
    }
    
    printf("[CORE0] sistem hazir, komut bekleniyor...\n");
    printf(">>> ");
    
    char input_buffer[256];
    int input_pos = 0;
    
    while (true) {
        // gelen veriyi isle
        rfd_node1.processIncomingData();
        
        // usb'den komut oku
        int c = getchar_timeout_us(1000); 
        if (c >= 0) {
            if (c == '\n' || c == '\r') {
                if (input_pos > 0) {
                    input_buffer[input_pos] = '\0';
                    parse_and_execute_command(input_buffer, rfd_node1);
                    input_pos = 0;
                    sleep_ms(10); 
                    printf(">>> ");
                }
            } else if (c == '\b' || c == 127) { // backspace
                if (input_pos > 0) {
                    input_pos--;
                    printf("\b \b");
                }
            } else if (input_pos < 255) {
                input_buffer[input_pos++] = c;
                printf("%c", c);
            }
        }
        
        sleep_ms(20);
    }
    
    return 0;
}
    
