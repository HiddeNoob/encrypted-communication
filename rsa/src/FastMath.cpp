#include "FastMath.h"

type FastMath::mod_inverse(type a, type m) {
    type x, y;
    type gcd = FastMath::extended_gcd(a, m, x, y);
    if (gcd != 1) {
        // Modular inverse does not exist
        return -1;
    } else {
        // Ensure result is positive (handle negative x)
        __int128_t sx = static_cast<__int128_t>(x);
        __int128_t sm = static_cast<__int128_t>(m);
        __int128_t result = ((sx % sm) + sm) % sm;
        return static_cast<type>(result);
    }
}

type FastMath::extended_gcd(type a, type b, type &x, type &y) {
    if (b == 0) {
        x = 1;
        y = 0;
        return a; // gcd
    }
    type x1, y1;
    type gcd = extended_gcd(b, a % b, x1, y1);
    
    x = y1;
    y = x1 - (a / b) * y1;
    
    return gcd;
}

type FastMath::gcd(type a, type b) {
    if (b == 0)
        return a;
    return gcd(b, a % b);
}

type FastMath::fast_pow(type a, type b, type mod) {
    type res = 1;
    a = a % mod;
    while (b > 0) {
        if (b & 1)
            res = (res * a) % mod;
        b >>= 1;
        a = (a * a) % mod;
    }
    return res;
}

type FastMath::totient(type p, type q) {
    return (p - 1) * (q - 1);
}
