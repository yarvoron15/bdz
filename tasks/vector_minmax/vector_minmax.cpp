#include "vector_minmax.h"

std::pair<IntVectorCIt, IntVectorCIt> iVectorMinMax(IntVectorCIt first,
                                                    IntVectorCIt last)
{
    IntVectorCIt min_it = first;
    IntVectorCIt max_it = first;

    for (IntVectorCIt it = first; it != last; ++it) {
        if (*it < *min_it) {
            min_it = it;
        }
        if (*max_it < *it) {
            max_it = it;
        }
    }

    return {min_it, max_it};
}
