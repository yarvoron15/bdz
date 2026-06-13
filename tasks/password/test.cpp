#include <catch2/catch_shim.hpp>

#include "password.h"

TEST_CASE("ValidatePassword") {
    REQUIRE(!validatePassword(""));
    REQUIRE(!validatePassword("qwerty"));

    REQUIRE(validatePassword("Aa1!Bb2@"));
    REQUIRE(validatePassword("Aa!Aa!Aa!"));
}
