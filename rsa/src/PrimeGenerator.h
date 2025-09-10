#pragma once
#include <set>

#include "config.h"

class PrimeGenerator {
public:
    PrimeGenerator(rsa_data limit);
    ~PrimeGenerator();
    rsa_data getRandomPrime();
    bool isPrime(rsa_data number);

private:
    bool* nums;
    rsa_data m_limit;
    std::set<rsa_data> primes;

    void calculatePrimes();
};
