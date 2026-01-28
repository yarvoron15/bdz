#include <catch2/catch_shim.hpp>

#include "fibonacci.h"

TEST_CASE("Simple") {
    REQUIRE(calculateNthFibonacci(0) == 0);
    REQUIRE(calculateNthFibonacci(1) == 1);
    REQUIRE(calculateNthFibonacci(2) == 1);
    REQUIRE(calculateNthFibonacci(3) == 2);
    REQUIRE(calculateNthFibonacci(4) == 3);
    REQUIRE(calculateNthFibonacci(5) == 5);
    REQUIRE(calculateNthFibonacci(6) == 8);
}

TEST_CASE("Big values") {
    REQUIRE(calculateNthFibonacci(80) == 23416728348467685);
}
