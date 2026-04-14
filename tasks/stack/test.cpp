#include <catch2/catch_shim.hpp>

#include "stack.h"

TEST_CASE("Simple")
{
    Stack s;
    s.push(0);
    s.push(1);
    s.push(2);
    REQUIRE(2 == s.top());
    s.pop();
    REQUIRE(1 == s.top());
    s.pop();
    REQUIRE(0 == s.top());
    s.pop();
    REQUIRE(s.empty());
}

TEST_CASE("Empty")
{
    Stack s;
    REQUIRE(s.empty());
    REQUIRE(0u == s.size());
    s.push(1);
    REQUIRE(!s.empty());
    REQUIRE(1u == s.size());
}

TEST_CASE("Long")
{
    Stack s;
    const int iterations = 5e4;
    for (int i = 0; i < iterations; ++i)
    {
        s.push(i);
    }
    for (int i = 0; i < iterations; ++i)
    {
        REQUIRE(iterations - i - 1 == s.top());
        s.pop();
    }
    REQUIRE(true == s.empty());
    REQUIRE(0u == s.size());
}
