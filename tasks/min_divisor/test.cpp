#include <catch2/catch_shim.hpp>

#include <chrono>

#include "min_divisor.h"

TEST_CASE("Simple") {
    REQUIRE(minDivisor(3) == 3);
    REQUIRE(minDivisor(2) == 2);
    REQUIRE(minDivisor(6) == 2);
    REQUIRE(minDivisor(12) == 2);
    REQUIRE(minDivisor(9) == 3);
}

TEST_CASE("Big prime") {
    const int64_t prime = 753454267;
    auto start = std::chrono::system_clock::now();
    REQUIRE(minDivisor(prime) == prime);
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    REQUIRE(duration < 100);
}
