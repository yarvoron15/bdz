#include "associative_operation.h"

bool isAssociative(const std::vector<std::vector<size_t> >& table)
{
    const size_t n = table.size();
    for (size_t x = 0; x < n; ++x) {
        for (size_t y = 0; y < n; ++y) {
            for (size_t z = 0; z < n; ++z) {
                if (table[table[x][y]][z] != table[x][table[y][z]]) {
                    return false;
                }
            }
        }
    }
    return true;
}
