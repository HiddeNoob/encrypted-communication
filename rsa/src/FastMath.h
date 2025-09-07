#pragma once
#include "config.h"

class FastMath {
public:
    static type mod_inverse(type a, type m);
    static type extended_gcd(type a, type b, type &x, type &y);
    static type gcd(type a, type b);
    static type fast_pow(type a, type b, type mod);
    static type totient(type p, type q);
};
