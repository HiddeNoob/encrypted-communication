#pragma once
#include <memory>

class FastMath {
public:
    static int64_t mod_inverse(int64_t a, int64_t m);
    static int64_t extended_gcd(int64_t a, int64_t b, int64_t &x, int64_t &y);
    static int64_t gcd(int64_t a, int64_t b);
    static int64_t fast_pow(int64_t a, int64_t b, int64_t mod);
    static int64_t totient(int64_t p, int64_t q);
    static void myHash(const uint8_t* data, uint32_t* hash, uint32_t length);
};