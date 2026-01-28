#ifndef FP16_H_
#define FP16_H_


#include <cstdint>

// Convert IEEE 754 half-precision float (encoded as uint16_t) to single-precision float.
//
// The float16 format uses 16 bits:
// - 1 bit for sign (bit 15)
// - 5 bits for exponent (bits 14-10) with bias 15
// - 10 bits for mantissa (bits 9-0)
//
// Special cases to handle:
// - Zero: exponent = 0, mantissa = 0
// - Denormalized: exponent = 0, mantissa != 0
// - Infinity: exponent = 31, mantissa = 0
// - NaN: exponent = 31, mantissa != 0
//
// Use only arithmetic and bitwise operations - no type-punning allowed!
float convertFloat16ToFloat(uint16_t float16_bits);


#endif // FP16_H_
