#include <catch2/catch_shim.hpp>
#include <numeric>

#include "provinces.h"

// NOLINTBEGIN
TEST_CASE("Simple")
{
    REQUIRE(3 == countPassports({1, 2}));

    REQUIRE(10 == countPassports({1, 2, 4}));
}

TEST_CASE("Stress")
{
    std::vector<int> provinces(1e6);
    std::iota(provinces.begin(), provinces.end(), 1);
    REQUIRE(9839463073984 == countPassports(provinces));
}
// NOLINTEND
