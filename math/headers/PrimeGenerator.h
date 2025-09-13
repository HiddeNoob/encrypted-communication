#pragma once
#include <set>
#include <cstdint>

class PrimeGenerator {
public:
    PrimeGenerator(uint64_t limit);
    ~PrimeGenerator();
    uint64_t getRandomPrime();
    bool isPrime(uint64_t number);

private:
    bool* nums;
    uint64_t m_limit;
    std::set<uint64_t> primes;

    void calculatePrimes();
};