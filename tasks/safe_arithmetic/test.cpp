#include <catch2/catch_shim.hpp>

#include "safe_arithmetic.h"

#include <limits>
#include <random>

TEST_CASE("SafeAdd basic operations")
{
    int64_t result = 42;

    SECTION("Simple positive addition")
    {
        CHECK(safeAdd(10, 20, result) == true);
        CHECK(result == 30);
    }

    SECTION("Addition with zero")
    {
        CHECK(safeAdd(100, 0, result) == true);
        CHECK(result == 100);

        CHECK(safeAdd(0, -50, result) == true);
        CHECK(result == -50);
    }

    SECTION("Negative numbers")
    {
        CHECK(safeAdd(-10, -20, result) == true);
        CHECK(result == -30);

        CHECK(safeAdd(-100, 150, result) == true);
        CHECK(result == 50);
    }
}

TEST_CASE("SafeAdd overflow detection")
{
    int64_t result = 42;
    const int64_t max_val = std::numeric_limits<int64_t>::max();
    const int64_t min_val = std::numeric_limits<int64_t>::min();

    SECTION("Positive overflow")
    {
        CHECK(safeAdd(max_val, 1, result) == false);
        CHECK(result == 42);

        CHECK(safeAdd(max_val, max_val, result) == false);
        CHECK(result == 42);
    }

    SECTION("Negative overflow")
    {
        CHECK(safeAdd(min_val, -1, result) == false);
        CHECK(result == 42);

        CHECK(safeAdd(min_val, min_val, result) == false);
        CHECK(result == 42);
    }

    SECTION("Boundary cases that should work")
    {
        CHECK(safeAdd(max_val, 0, result) == true);
        CHECK(result == max_val);

        CHECK(safeAdd(min_val, 0, result) == true);
        CHECK(result == min_val);

        CHECK(safeAdd(max_val, min_val, result) == true);
        CHECK(result == -1);
    }
}

TEST_CASE("SafeSubtract basic operations")
{
    int64_t result = 42;

    SECTION("Simple subtraction")
    {
        CHECK(safeSubtract(20, 10, result) == true);
        CHECK(result == 10);

        CHECK(safeSubtract(10, 20, result) == true);
        CHECK(result == -10);
    }

    SECTION("Subtraction with zero")
    {
        CHECK(safeSubtract(100, 0, result) == true);
        CHECK(result == 100);

        CHECK(safeSubtract(0, 50, result) == true);
        CHECK(result == -50);
    }

    SECTION("Negative numbers")
    {
        CHECK(safeSubtract(-10, -20, result) == true);
        CHECK(result == 10);

        CHECK(safeSubtract(-20, 10, result) == true);
        CHECK(result == -30);
    }
}

TEST_CASE("SafeSubtract overflow detection")
{
    int64_t result = 42;
    const int64_t max_val = std::numeric_limits<int64_t>::max();
    const int64_t min_val = std::numeric_limits<int64_t>::min();

    SECTION("Positive overflow")
    {
        CHECK(safeSubtract(max_val, -1, result) == false);
        CHECK(result == 42);

        CHECK(safeSubtract(max_val, min_val, result) == false);
        CHECK(result == 42);
    }

    SECTION("Negative overflow")
    {
        CHECK(safeSubtract(min_val, 1, result) == false);
        CHECK(result == 42);

        CHECK(safeSubtract(min_val, max_val, result) == false);
        CHECK(result == 42);
    }

    SECTION("Boundary cases that should work")
    {
        CHECK(safeSubtract(max_val, 0, result) == true);
        CHECK(result == max_val);

        CHECK(safeSubtract(min_val, 0, result) == true);
        CHECK(result == min_val);

        CHECK(safeSubtract(max_val, max_val, result) == true);
        CHECK(result == 0);
    }
}

TEST_CASE("SafeMultiply basic operations")
{
    int64_t result = 42;

    SECTION("Simple multiplication")
    {
        CHECK(safeMultiply(10, 20, result) == true);
        CHECK(result == 200);

        CHECK(safeMultiply(-10, 20, result) == true);
        CHECK(result == -200);

        CHECK(safeMultiply(-10, -20, result) == true);
        CHECK(result == 200);
    }

    SECTION("Multiplication with zero")
    {
        CHECK(safeMultiply(0, 1000000000000LL, result) == true);
        CHECK(result == 0);

        CHECK(safeMultiply(1000000000000LL, 0, result) == true);
        CHECK(result == 0);
    }

    SECTION("Multiplication with one")
    {
        CHECK(safeMultiply(123456789, 1, result) == true);
        CHECK(result == 123456789);

        CHECK(safeMultiply(-123456789, 1, result) == true);
        CHECK(result == -123456789);
    }
}

TEST_CASE("SafeMultiply overflow detection")
{
    int64_t result = 42;
    const int64_t max_val = std::numeric_limits<int64_t>::max();
    const int64_t min_val = std::numeric_limits<int64_t>::min();

    SECTION("Large positive numbers")
    {
        CHECK(safeMultiply(max_val, 2, result) == false);
        CHECK(result == 42);

        CHECK(safeMultiply(1000000000000LL, 1000000000000LL, result) == false);
        CHECK(result == 42);
    }

    SECTION("Large negative numbers")
    {
        CHECK(safeMultiply(min_val, 2, result) == false);
        CHECK(result == 42);

        CHECK(safeMultiply(-1000000000000LL, 1000000000000LL, result) == false);
        CHECK(result == 42);
    }

    SECTION("Edge case: min_val * -1")
    {
        CHECK(safeMultiply(min_val, -1, result) == false);
        CHECK(result == 42);
    }

    SECTION("Boundary cases that should work")
    {
        CHECK(safeMultiply(max_val, 1, result) == true);
        CHECK(result == max_val);

        CHECK(safeMultiply(min_val, 1, result) == true);
        CHECK(result == min_val);

        // Square root of max_val should be safe to multiply
        int64_t sqrt_max = 3037000499LL;  // approximately sqrt(INT64_MAX)
        CHECK(safeMultiply(sqrt_max, sqrt_max, result) == true);
    }
}

TEST_CASE("SafeDivide basic operations")
{
    int64_t result = 42;

    SECTION("Simple division")
    {
        CHECK(safeDivide(20, 10, result) == true);
        CHECK(result == 2);

        CHECK(safeDivide(-20, 10, result) == true);
        CHECK(result == -2);

        CHECK(safeDivide(-20, -10, result) == true);
        CHECK(result == 2);
    }

    SECTION("Division with remainder")
    {
        CHECK(safeDivide(23, 10, result) == true);
        CHECK(result == 2);

        CHECK(safeDivide(-23, 10, result) == true);
        CHECK(result == -2);
    }

    SECTION("Division by one")
    {
        CHECK(safeDivide(123456789, 1, result) == true);
        CHECK(result == 123456789);

        CHECK(safeDivide(-123456789, 1, result) == true);
        CHECK(result == -123456789);
    }
}

TEST_CASE("SafeDivide error cases")
{
    int64_t result = 42;
    const int64_t min_val = std::numeric_limits<int64_t>::min();

    SECTION("Division by zero")
    {
        CHECK(safeDivide(100, 0, result) == false);
        CHECK(result == 42);

        CHECK(safeDivide(-100, 0, result) == false);
        CHECK(result == 42);

        CHECK(safeDivide(0, 0, result) == false);
        CHECK(result == 42);
    }

    SECTION("The only overflow case: INT64_MIN / -1")
    {
        CHECK(safeDivide(min_val, -1, result) == false);
        CHECK(result == 42);
    }

    SECTION("Safe cases near boundaries")
    {
        CHECK(safeDivide(min_val, 1, result) == true);
        CHECK(result == min_val);

        CHECK(safeDivide(min_val, 2, result) == true);
        CHECK(result == min_val / 2);

        CHECK(safeDivide(min_val, -2, result) == true);
        CHECK(result == min_val / -2);
    }
}

TEST_CASE("All operations with large numbers")
{
    int64_t result = 42;

    // Use some large but safe numbers
    const int64_t large_pos = 1000000000000000LL;  // 10^15
    const int64_t large_neg = -1000000000000000LL;

    SECTION("Large safe additions")
    {
        CHECK(safeAdd(large_pos, large_pos, result) == true);
        CHECK(result == 2 * large_pos);

        CHECK(safeAdd(large_neg, large_neg, result) == true);
        CHECK(result == 2 * large_neg);
    }

    SECTION("Large safe subtractions")
    {
        CHECK(safeSubtract(large_pos, large_neg, result) == true);
        CHECK(result == large_pos - large_neg);
    }

    SECTION("Large multiplications should overflow")
    {
        CHECK(safeMultiply(large_pos, large_pos, result) == false);
        CHECK(result == 42);  // Should not be modified
    }

    SECTION("Large divisions should work")
    {
        CHECK(safeDivide(large_pos, 1000, result) == true);
        CHECK(result == large_pos / 1000);
    }
}

// Quod licet Iovi, non licet bovi.
template <template <typename> typename Op>
bool ReallySafeOp(int64_t a, int64_t b, int64_t& result)
{
    __int128_t wide_result = Op<__int128_t>{}(static_cast<__int128_t>(a), static_cast<__int128_t>(b));
    if (wide_result != static_cast<int64_t>(wide_result))
        return false;

    result = static_cast<int64_t>(wide_result);
    return true;
}

TEST_CASE("Stress test")
{
    std::mt19937_64 rng(42);
    std::uniform_int_distribution<int64_t> dist {
                        std::numeric_limits<int64_t>::min(),
                        std::numeric_limits<int64_t>::max()
    };

    for (int i = 0; i < 10'000'000; ++i)
    {
        int64_t a = dist(rng);
        int64_t b = dist(rng);

        int64_t result = 42;
        int64_t expected = 42;

        REQUIRE(ReallySafeOp<std::plus>(a, b, expected) == safeAdd(a, b, result));
        REQUIRE(expected == result);

        REQUIRE(ReallySafeOp<std::minus>(a, b, expected) == safeSubtract(a, b, result));
        REQUIRE(expected == result);

        REQUIRE(ReallySafeOp<std::multiplies>(a, b, expected) == safeMultiply(a, b, result));
        REQUIRE(expected == result);

        REQUIRE(ReallySafeOp<std::divides>(a, b, expected) == safeDivide(a, b, result));
        REQUIRE(expected == result);
    }
}
