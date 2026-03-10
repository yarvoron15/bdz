#include <catch2/catch_shim.hpp>

#include "vector_minmax.h"

TEST_CASE("Public Simple") {
    {
        IntVector a{1, 3, 2};
        auto [min, max] = iVectorMinMax(a.begin(), a.end());
        REQUIRE(*min == 1);
        REQUIRE(*max == 3);
    }

    {
        IntVector a{1, -3, 2};
        auto [min, max] = iVectorMinMax(a.begin(), a.end());
        REQUIRE(*min == -3);
        REQUIRE(*max == 2);
    }
}

// This approach will be available later when a templated version of the task
// is ready.
// // NOLINTBEGIN
// TEST_CASE("Public Other types") {
//     {
//         int a[5] = {1, 5, 3, 4, 2};
//         auto [min, max] = iVectorMinMax(a, a + 5);
//         REQUIRE(*min == 1);
//         REQUIRE(*max == 5);
//     }
// }
// // NOLINTEND
