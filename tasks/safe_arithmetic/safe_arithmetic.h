#ifndef SAFE_ARITHMETIC_H_
#define SAFE_ARITHMETIC_H_


#include <cstdint>

bool safeAdd(int64_t a, int64_t b, int64_t& result);

bool safeSubtract(int64_t a, int64_t b, int64_t& result);

bool safeMultiply(int64_t a, int64_t b, int64_t& result);

bool safeDivide(int64_t a, int64_t b, int64_t& result);


#endif // SAFE_ARITHMETIC_H_
