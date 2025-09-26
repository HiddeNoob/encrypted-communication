#include "Key.h"
#include "FastMath.h"
#include <cstdio>
#include <cstring>
Key::Key(rsa_data key,rsa_data base) : key(key), base(base){}

rsa_data* Key::sign(void* data,size_t length,size_t byte_size){
    printf("[Key] Signing %zu elements with key %llx and base %llx\n", length, (unsigned long long)key, (unsigned long long)base);
    rsa_data* signature = new rsa_data[length];
    // Her elemanı byte_size kadar ilerleyerek işliyoruz
    for(size_t i = 0; i < length; i++){
        // data pointer'ını byte_size ile ilerletip, unsigned long long olarak al
        void* element_ptr = (void*)((char*)data + i * byte_size);
        rsa_data value = 0;
        // byte_size'a göre veriyi value'ya kopyala
        memcpy(&value, element_ptr, byte_size);

        //imzala
        signature[i] = FastMath::fast_pow(value, key, base);
        printf("Signing value %llx -> %llx\n", value, signature[i]);
    }
    return signature;
}