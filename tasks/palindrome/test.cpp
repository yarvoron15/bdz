#include <catch2/catch_shim.hpp>

#include "palindrome.h"

TEST_CASE("IsPalindrome") {
    REQUIRE(isPalindrome("abba"));
    REQUIRE(isPalindrome("a roza upala na lapu azora"));

    REQUIRE(!isPalindrome("qwerty"));
}
