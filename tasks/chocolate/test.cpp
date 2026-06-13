#include <catch2/catch_shim.hpp>

#include "chocolate.h"

TEST_CASE("Simple") {
    REQUIRE(canBreakSlices(2, 1, 1));
    REQUIRE(canBreakSlices(4, 12, 8));

    REQUIRE(!canBreakSlices(2, 2, 4));
    REQUIRE(!canBreakSlices(3, 3, 7));
}
