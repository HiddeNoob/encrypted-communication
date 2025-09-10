#pragma once
#include "config.h"

class FastMath {
public:
    static rsa_data mod_inverse(rsa_data a, rsa_data m);
    static rsa_data extended_gcd(rsa_data a, rsa_data b, rsa_data &x, rsa_data &y);
    static rsa_data gcd(rsa_data a, rsa_data b);
    static rsa_data fast_pow(rsa_data a, rsa_data b, rsa_data mod);
    static rsa_data totient(rsa_data p, rsa_data q);
};
