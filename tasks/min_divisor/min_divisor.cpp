#include "min_divisor.h"

int64_t minDivisor(int64_t number)
{
    if (number % 2 == 0) {
        return 2;
    }

    for (int64_t divisor = 3; divisor <= number / divisor; divisor += 2) {
        if (number % divisor == 0) {
            return divisor;
        }
    }
    return number;
}
