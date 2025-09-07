#include "RSA.h"
#include <cstring>
#include "pico/stdlib.h"
#include "pico/stdio.h"

bool test_rsa_encryption_decryption(int bits, const char* test_data) {
    printf("\n=== Testing RSA with %d bits ===\n", bits);
    
    RSA cryption = RSA(bits);
    int data_len = strlen(test_data);
    type* encrypted = new type[data_len];
    char* decrypted = new char[data_len + 1];

    printf("Original data: %s\n", test_data);
    
    // Encrypt
    cryption.Encrypt(test_data, data_len, encrypted);

    printf("Encrypted data: ");
    for (int i = 0; i < data_len; ++i) {
        printf("%llx ", (unsigned long long)(encrypted[i]));
    }
    printf("\n");

    // Decrypt
    cryption.Decrypt(encrypted, data_len, decrypted);
    decrypted[data_len] = '\0';
    printf("Decrypted data: %s\n", decrypted);

    // Verify
    bool success = (strcmp(test_data, decrypted) == 0);
    printf("Test result: %s\n", success ? "PASSED" : "FAILED");
    
    if (!success) {
        printf("ERROR: Decryption failed!\n");
        printf("Expected: %s\n", test_data);
        printf("Got: %s\n", decrypted);
    }

    delete[] encrypted;
    delete[] decrypted;
    return success;

}

int main() {
    printf("RSA Encryption/Decryption Test Suite\n");
    printf("=====================================\n");
    
    int passed = 0;
    int total = 0;

    int bit = 16;
    
    // Test 1: Basic text
    total++;
    if (test_rsa_encryption_decryption(bit, "Hello, World!")) {
        passed++;
    }
    
    // Test 2: Single character
    total++;
    if (test_rsa_encryption_decryption(bit, "A")) {
        passed++;
    }
    
    // Test 3: Numbers and symbols
    total++;
    if (test_rsa_encryption_decryption(bit, "12345!@#$%")) {
        passed++;
    }
    
    // Test 4: Empty string
    total++;
    if (test_rsa_encryption_decryption(bit, "")) {
        passed++;
    }
    
    // Test 5: Longer text
    total++;
    if (test_rsa_encryption_decryption(bit, "This is a longer test message for RSA encryption.")) {
        passed++;
    }
    
    // Test 6: Different bit size
    total++;
    if (test_rsa_encryption_decryption(bit / 2, "Small key test")) {
        passed++;
    }
    
    printf("\n=====================================\n");
    printf("Test Results: %d/%d tests passed\n", passed, total);
    
    if (passed == total) {
        printf("All tests PASSED! RSA implementation is working correctly.\n");
        return 0;
    } else {
        printf("Some tests FAILED! Check RSA implementation.\n");
        return 1;
    }
}