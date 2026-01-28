#include <catch2/catch_shim.hpp>

#include "next_even.h"

TEST_CASE("Simple") {
    REQUIRE(4 == nextEven(3));
    REQUIRE(4 == nextEven(2));
}
