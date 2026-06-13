#include "next_even.h"

int64_t nextEven(int64_t n)
{
    return n + 2 - (n % 2);
}
