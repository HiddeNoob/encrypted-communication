#include <iostream>
#include <vector>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/mutex.h"
#include <cstring>
#include "RSA.h"
#include "RFD.h"
#include "Node.h"

volatile bool core1_ready = false;
mutex_t printf_mutex;

void show_menu() {
    printf("\n=== RFD Ag Yonetimi Menusu ===\n");
    printf("1. Agi tara (Hello paketi gonder)\n");
    printf("2. Bagli olan nodelar\n");
    printf("0. Cikis\n");
    printf("Seciminiz (0-2): ");
}

void show_connected_nodes_menu(Node& node) {
    const auto& network = node.getNetwork();
    if (network.empty()) {
        printf("\nHenuz bagli node yok. Once ag taramasi yapin (secim 1).\n");
        return;
    }
    
    printf("\n=== Bagli Nodlar (%zu adet) ===\n", network.size());
    
    // Node'ları vector'e koyup indeksle erişim sağlayalım
    std::vector<std::pair<uint64_t, RemoteNode*>> node_list;
    for (const auto& pair : network) {
        node_list.push_back(pair);
    }
    
    // Node'ları listele
    for (size_t i = 0; i < node_list.size(); i++) {
        printf("%zu. Node ID: %llx\n", i+1, node_list[i].first);
        printf("   Public Key: %llx\n", node_list[i].second->getPublicKey().getKey());
        printf("   Modulus: %llx\n", node_list[i].second->getPublicKey().getBase());
        printf("\n");
    }
    
    printf("=== Node Secimi ===\n");
    printf("1-%zu: Node sec\n", node_list.size());
    printf("0: Ana menuye don\n");
    printf("Hangi node'u seciyorsunuz: ");
    
    int c = getchar();
    if (c != EOF) {
        int next;
        while ((next = getchar_timeout_us(10)) != EOF && next != '\n' && next != '\r') {}
        printf("%c\n", c);
        
        if (c == '0') {
            return;
        }
        else if (c >= '1' && c <= '9') {
            int node_index = c - '1'; // 1-based'den 0-based'e çevir
            if (node_index < (int)node_list.size()) {
                uint64_t target_id = node_list[node_index].first;
                printf("\nNode %zu (%llx) secildi.\n", node_index + 1, target_id);
                
                // İşlem seçimi
                while(true){

                    printf("\n=== Islem Secimi ===\n");
                    printf("p: Ping gonder\n");
                    printf("m: Mesaj gonder\n");
                    printf("0: Geri don\n");
                    printf("Seciminiz: ");
                    
                    int operation = getchar();
                    if (operation != EOF) {
                        int next;
                        while ((next = getchar_timeout_us(10)) != EOF && next != '\n' && next != '\r') {}
                        printf("%c\n", operation);
                        
                        if (operation == 'p' || operation == 'P') {
                            printf("Node %llx'e ping gonderiliyor...\n", target_id);
                            node.getRFD().sendPing(target_id);
                            printf("Ping gonderildi!\n");
                        }
                        else if (operation == 'm' || operation == 'M') {
                            printf("Node %llx'e mesaj gondereceksiniz.\n", target_id);
                            printf("Mesajinizi yazin ve Enter'a basin (max 200 karakter):\n");
                            
                            char message[201];
                            message[0] = '\0';
                            int pos = 0;
                            
                            // Mesaj inputu
                            while (pos < 200) {
                                int msg_c = getchar();
                                if (msg_c == '\n' || msg_c == '\r') {
                                    break;
                                }
                                if (msg_c != EOF) {
                                    message[pos++] = msg_c;
                                    printf("%c", msg_c); // Echo
                                }
                            }
                            message[pos] = '\0';
                            printf("\n");
                            
                            if (pos > 0) {
                                node.getRFD().sendMessage(target_id, message);
                            } else {
                                printf("Bos mesaj gonderilemez!\n");
                            }
                        }
                        else if (operation == '0') {
                            printf("Node secim menusune donuluyor...\n");
                            break;
                        }
                        else {
                            printf("Gecersiz islem!\n");
                        }
                    }

                }
            } else {
                printf("Gecersiz node numarasi!\n");
            }
        }
        else {
            printf("Gecersiz secim!\n");
        }
    }
}

void core1_main() {
    Node node2(RFD(uart1, 4, 5));
    
    core1_ready = true;
    
    mutex_enter_blocking(&printf_mutex);
    printf("[CORE1] node2 hazir, id: %llx\n", node2.getId());
    mutex_exit(&printf_mutex);
    
    while (true) {
        node2.getRFD().processIncomingData(node2);
        sleep_ms(1);
    }
}

int main() {
    stdio_init_all();
    sleep_ms(2000);

    srand(time_us_64());

    mutex_init(&printf_mutex);
    
    printf("=== rfd ag yonetimi sistemi ===\n");
    printf("node1: uart0 (gpio 0-1)\n");
    printf("node2: uart1 (gpio 4-5)\n");
    
    multicore_launch_core1(core1_main);
    
    Node node1(RFD(uart0, 0, 1));
    
    printf("[CORE0] node1 hazir, id: %llx\n", node1.getId());
    
    while (!core1_ready) {
        sleep_ms(10);
    }
    
    printf("[CORE0] sistem hazir!\n");

    show_menu();
    
    while (true) {
        node1.getRFD().processIncomingData(node1);
        
        int c = getchar_timeout_us(1000);
        if (c != EOF && c != PICO_ERROR_TIMEOUT) {
            int next;
            while ((next = getchar_timeout_us(10)) != EOF && next != '\n' && next != '\r') {}
            
            printf("%c\n", (char)c);
            
            switch (c) {
                case '1':
                    node1.getRFD().sendHello();
                    break;
                case '2':
                    show_connected_nodes_menu(node1);
                    break;
                case '0':
                    printf("Sistem kapatiliyor...\n");
                    return 0;
                case '\n':
                case '\r':
                    break;
                default:
                    printf("Gecersiz secim! Lutfen 0, 1 veya 2 girin.\n");
                    show_menu();
                    break;
            }
        }
    }
    return 0;
}