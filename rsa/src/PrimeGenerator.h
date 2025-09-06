#pragma once
#include <set>

class PrimeGenerator {
public:
    PrimeGenerator(__uint128_t limit);
    ~PrimeGenerator();
    __uint128_t getRandomPrime();
    bool isPrime(__uint128_t number);

private:
    bool* nums;
    __uint128_t m_limit;
    std::set<__uint128_t> primes;

    void calculatePrimes();
};
