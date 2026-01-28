#include <catch2/catch_shim.hpp>

#include "multiplication.h"

TEST_CASE("Simple") {
    REQUIRE(6 == multiply(2, 3));
}

TEST_CASE("Advanced") {
    REQUIRE(-100000018299999867LL == multiply(999999993, -100000019));
}
