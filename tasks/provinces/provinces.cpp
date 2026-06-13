#include "provinces.h"

#include <queue>

int64_t countPassports(const std::vector<int>& provinces)
{
    std::priority_queue<int64_t, std::vector<int64_t>, std::greater<int64_t>> min_heap;
    for (int population : provinces) {
        min_heap.push(population);
    }

    int64_t total = 0;
    while (min_heap.size() > 1) {
        const int64_t first = min_heap.top();
        min_heap.pop();
        const int64_t second = min_heap.top();
        min_heap.pop();

        const int64_t merged = first + second;
        total += merged;
        min_heap.push(merged);
    }

    return total;
}
