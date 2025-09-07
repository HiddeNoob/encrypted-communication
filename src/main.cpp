#include <iostream>
#include "pico/stdlib.h"
#include <cstring>
#include "RSA.h"
// #include "RFD.h"  // Gelecekte RFD kütüphanesi eklendiğinde

void printArray(type *ptr,type len){
    while(--len >= 0) printf("%llx ",*(ptr++));
}

int main() {

    const char data[] = "selam!";
    int data_length = strlen(data);
    stdio_init_all();
    type encrypted_data[data_length];
    RSA rsa(16);
    printf("RSA init completed");
    sleep_ms(5000);
    char decrypted_data[data_length];
    while (true)
    {
        
    printf("data: %s\n",data);
    printf("encrypt basliyor\n");
    uint32_t t0 = time_us_32();
    rsa.Encrypt(data,data_length,encrypted_data);
    uint32_t t1 = time_us_32();
    printf("encryped data\n");
    printArray(encrypted_data,data_length);
    printf("\n");
    printf("decrypt basliyor\n");
    uint32_t t2 = time_us_32();
    rsa.Decrypt(encrypted_data,data_length,decrypted_data);
    uint32_t t3 = time_us_32();
    printf("decrypted data: %s\n",decrypted_data);
    printf("\nEncrypt süresi: %u us\n", t1-t0);
    printf("Decrypt süresi: %u us\n", t3-t2);
    sleep_ms(5000);
    }
    
    return 0;
}