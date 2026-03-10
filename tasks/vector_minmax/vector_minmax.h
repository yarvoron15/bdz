#ifndef VECTOR_MINMAX_H_
#define VECTOR_MINMAX_H_


#include <vector>

using IntVector = std::vector<int>;
using IntVectorCIt = IntVector::const_iterator;

std::pair<IntVectorCIt, IntVectorCIt> iVectorMinMax(IntVectorCIt first,
                                                    IntVectorCIt last);


#endif // VECTOR_MINMAX_H_
