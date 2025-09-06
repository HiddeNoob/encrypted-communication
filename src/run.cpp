#include <iostream>
#include "RSA.h"
// #include "RFD.h"  // Gelecekte RFD kütüphanesi eklendiğinde

int main() {
    std::cout << "Blink Simple Project Starting..." << std::endl;
    
    // RSA test
    std::cout << "Testing RSA Library:" << std::endl;
    RSA rsa(32);
    
    // RFD kütüphanesi gelecekte buraya eklenecek
    // std::cout << "Testing RFD Library:" << std::endl;
    
    std::cout << "Project completed successfully!" << std::endl;
    return 0;
}