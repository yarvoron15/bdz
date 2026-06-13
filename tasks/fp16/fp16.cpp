#include "fp16.h"

#include <cmath>
#include <limits>

float convertFloat16ToFloat(uint16_t float16_bits)
{
    const bool negative = (float16_bits & 0x8000U) != 0;
    const uint16_t exponent = (float16_bits >> 10U) & 0x1FU;
    const uint16_t mantissa = float16_bits & 0x03FFU;

    float value = 0.0f;
    if (exponent == 0) {
        if (mantissa == 0) {
            value = 0.0f;
        } else {
            value = std::ldexp(static_cast<float>(mantissa), -24);
        }
    } else if (exponent == 0x1FU) {
        if (mantissa == 0) {
            value = std::numeric_limits<float>::infinity();
        } else {
            value = std::numeric_limits<float>::quiet_NaN();
        }
    } else {
        value = std::ldexp(1.0f + static_cast<float>(mantissa) / 1024.0f,
                           static_cast<int>(exponent) - 15);
    }

    return negative ? -value : value;
}
