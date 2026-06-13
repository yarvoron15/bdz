#include "safe_arithmetic.h"

#include <limits>

bool safeAdd(int64_t a, int64_t b, int64_t& result)
{
    if ((b > 0 && a > std::numeric_limits<int64_t>::max() - b) ||
        (b < 0 && a < std::numeric_limits<int64_t>::min() - b)) {
        return false;
    }
    result = a + b;
    return true;
}

bool safeSubtract(int64_t a, int64_t b, int64_t& result)
{
    if ((b > 0 && a < std::numeric_limits<int64_t>::min() + b) ||
        (b < 0 && a > std::numeric_limits<int64_t>::max() + b)) {
        return false;
    }
    result = a - b;
    return true;
}

bool safeMultiply(int64_t a, int64_t b, int64_t& result)
{
    if (a == 0 || b == 0) {
        result = 0;
        return true;
    }

    const int64_t max_value = std::numeric_limits<int64_t>::max();
    const int64_t min_value = std::numeric_limits<int64_t>::min();

    if ((a == -1 && b == min_value) || (b == -1 && a == min_value)) {
        return false;
    }

    if (a > 0) {
        if ((b > 0 && a > max_value / b) || (b < 0 && b < min_value / a)) {
            return false;
        }
    } else {
        if ((b > 0 && a < min_value / b) || (b < 0 && a < max_value / b)) {
            return false;
        }
    }

    result = a * b;
    return true;
}

bool safeDivide(int64_t a, int64_t b, int64_t& result)
{
    if (b == 0) {
        return false;
    }
    if (a == std::numeric_limits<int64_t>::min() && b == -1) {
        return false;
    }
    result = a / b;
    return true;
}
