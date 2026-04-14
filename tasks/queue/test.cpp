#include <catch2/catch_shim.hpp>

#include "queue.h"

TEST_CASE("Simple")
{
    Queue s;
    s.push(0);
    s.push(1);
    s.push(2);
    REQUIRE(0 == s.front());
    s.pop();
    REQUIRE(1 == s.front());
    s.pop();
    REQUIRE(2 == s.front());
    s.pop();
    REQUIRE(s.empty());
}

TEST_CASE("Empty")
{
    Queue s;
    REQUIRE(s.empty());
    REQUIRE(0u == s.size());
    s.push(1);
    REQUIRE(!s.empty());
    REQUIRE(1u == s.size());
}

TEST_CASE("Long")
{
    Queue s;
    const int iterations = 5e4;
    for (int i = 0; i < iterations; ++i)
    {
        s.push(i);
    }
    for (int i = 0; i < iterations; ++i)
    {
        REQUIRE(i == s.front());
        s.pop();
    }
    REQUIRE(true == s.empty());
    REQUIRE(0u == s.size());
}
