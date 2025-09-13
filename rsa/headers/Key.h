#pragma once
#include "config.h"
#include <cstdint>
#include <cstddef>

class Key{
    private:
        rsa_data key;
        rsa_data base;
    public:
        Key(rsa_data key,rsa_data base);
        rsa_data* sign(void* data,size_t length,size_t byte_size);
        rsa_data getKey() const { return key; }
        rsa_data getBase() const { return base; }
};