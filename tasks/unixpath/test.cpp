#include <catch2/catch_shim.hpp>

#include "unixpath.h"

TEST_CASE("NormalizePath")
{
    REQUIRE("/" == normalizePath("/", "."));
    REQUIRE("/home/user2" == normalizePath("/home/user1", "../user2"));

    REQUIRE("/" == normalizePath("/", ".."));
    REQUIRE("/tmp" == normalizePath("/home", "../../tmp"));

    REQUIRE("/a" == normalizePath("/", "../../a/"));
    REQUIRE("/b" == normalizePath("/", ".././/././/./../b/././././././"));
}
