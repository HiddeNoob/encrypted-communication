#include "FastMath.h"

// https://www.derspresso.com.tr/matematik/moduler-aritmetik/carpma-tersi
int64_t FastMath::mod_inverse(int64_t a, int64_t m) {
    int64_t x, y;
    int64_t gcd = FastMath::extended_gcd(a, m, x, y);
    if (gcd != 1) {
        // Modular inverse does not exist
        return -1;
    } else {
        // Ensure result is positive (handle negative x)
        int64_t sx = static_cast<int64_t>(x);
        int64_t sm = static_cast<int64_t>(m);
        int64_t result = ((sx % sm) + sm) % sm;
        return static_cast<int64_t>(result);
    }
}

// https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm
int64_t FastMath::extended_gcd(int64_t a, int64_t b, int64_t &x, int64_t &y) {
    if (b == 0) {
        x = 1;
        y = 0;
        return a; // gcd
    }
    int64_t x1, y1;
    int64_t gcd = extended_gcd(b, a % b, x1, y1);
    
    x = y1;
    y = x1 - (a / b) * y1;
    
    return gcd;
}

int64_t FastMath::gcd(int64_t a, int64_t b) {
    if (b == 0)
        return a;
    return gcd(b, a % b);
}

int64_t FastMath::fast_pow(int64_t a, int64_t b, int64_t mod) {
    int64_t res = 1;
    a = a % mod;
    while (b > 0) {
        if (b & 1) // tek olanı çarp
            res = (res * a) % mod;
        // karesini hesapla
        b >>= 1;
        a = (a * a) % mod;
    }
    return res;
}

// Ancak ve Ancak P ve Q asal ise
int64_t FastMath::totient(int64_t p, int64_t q) {
    return (p - 1) * (q - 1);
}

// güvenli degil, öğrenme amaçlı basit hash
void FastMath::myHash(const uint8_t* data, uint32_t* hash, uint32_t length) {
    *hash = 0x811C9DC5; 

    for (uint32_t i = 0; i < length; i++) {
        *hash ^= data[i];
        *hash *= 2654435761;
    }
}