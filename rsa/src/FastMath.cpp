#include "FastMath.h"

// https://www.derspresso.com.tr/matematik/moduler-aritmetik/carpma-tersi
rsa_data FastMath::mod_inverse(rsa_data a, rsa_data m) {
    rsa_data x, y;
    rsa_data gcd = FastMath::extended_gcd(a, m, x, y);
    if (gcd != 1) {
        // Modular inverse does not exist
        return -1;
    } else {
        // Ensure result is positive (handle negative x)
        rsa_data sx = static_cast<rsa_data>(x);
        rsa_data sm = static_cast<rsa_data>(m);
        rsa_data result = ((sx % sm) + sm) % sm;
        return static_cast<rsa_data>(result);
    }
}

// https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm
rsa_data FastMath::extended_gcd(rsa_data a, rsa_data b, rsa_data &x, rsa_data &y) {
    if (b == 0) {
        x = 1;
        y = 0;
        return a; // gcd
    }
    rsa_data x1, y1;
    rsa_data gcd = extended_gcd(b, a % b, x1, y1);
    
    x = y1;
    y = x1 - (a / b) * y1;
    
    return gcd;
}

rsa_data FastMath::gcd(rsa_data a, rsa_data b) {
    if (b == 0)
        return a;
    return gcd(b, a % b);
}

rsa_data FastMath::fast_pow(rsa_data a, rsa_data b, rsa_data mod) {
    rsa_data res = 1;
    a = a % mod;
    while (b > 0) {
        if (b & 1)
            res = (res * a) % mod;
        b >>= 1;
        a = (a * a) % mod;
    }
    return res;
}

// Ancak ve Ancak P ve Q asal ise
rsa_data FastMath::totient(rsa_data p, rsa_data q) {
    return (p - 1) * (q - 1);
}
