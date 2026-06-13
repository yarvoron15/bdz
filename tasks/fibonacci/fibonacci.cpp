#include "fibonacci.h"

int64_t calculateNthFibonacci(int64_t n)
{
    if (n == 0) {
        return 0;
    }

    int64_t prev = 0;
    int64_t curr = 1;
    for (int64_t i = 1; i < n; ++i) {
        const int64_t next = prev + curr;
        prev = curr;
        curr = next;
    }
    return curr;
}
