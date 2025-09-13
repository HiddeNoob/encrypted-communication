#include "PrimeGenerator.h"
#include <cstdlib>


PrimeGenerator::PrimeGenerator(uint64_t limit) {
    m_limit = limit;
    nums = new bool[limit];
    for (size_t i = 0; i < limit; ++i) nums[i] = true;
    calculatePrimes();
}

PrimeGenerator::~PrimeGenerator() {
    delete[] nums;
}

// asal olanların katlarını eleyerek asal sayı tablosu çıkarır
// TODO yüksek güvenlikli şifreleme için yetersiz, başka bir çözüm bulunmalı
// 2^32'de bile aşşırı yavaş çalışıyor.
void PrimeGenerator::calculatePrimes() {
    nums[0] = nums[1] = false;
    for (size_t i = 2; i * i < m_limit; i++) {
        if (nums[i]) {
            primes.insert(i);
            for (size_t j = i * i; j < m_limit; j += i) {
                nums[j] = false;
            }
        }
    }
}

uint64_t PrimeGenerator::getRandomPrime() {
    auto it = primes.begin(); // start pointer
    std::advance(it, rand() % primes.size()); // move right random (güvenli değil)
    return *it; // return that value
}

bool PrimeGenerator::isPrime(uint64_t number) {
    return nums[number];
}