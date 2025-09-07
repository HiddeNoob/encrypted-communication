#pragma once
#include <set>

#include "config.h"

class PrimeGenerator {
public:
    PrimeGenerator(type limit);
    ~PrimeGenerator();
    type getRandomPrime();
    bool isPrime(type number);

private:
    bool* nums;
    type m_limit;
    std::set<type> primes;

    void calculatePrimes();
};
