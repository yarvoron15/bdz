#include <catch2/catch_shim.hpp>

#include "fp16.h"

#include <cmath>

bool AreFloatsEqual(float a, float b, float epsilon = 1e-6f) {
    if (std::isnan(a) && std::isnan(b)) {
        return true;
    }
    if (std::isinf(a) && std::isinf(b)) {
        return (a > 0) == (b > 0);
    }
    return std::abs(a - b) < epsilon;
}

TEST_CASE("ConvertFloat16ToFloat") {
    SECTION("Zero values") {
        // Positive zero: 0x0000
        CHECK(convertFloat16ToFloat(0x0000) == 0.0f);
        CHECK(!std::signbit(convertFloat16ToFloat(0x0000)));

        // Negative zero: 0x8000
        CHECK(convertFloat16ToFloat(0x8000) == -0.0f);
        CHECK(std::signbit(convertFloat16ToFloat(0x8000)));
    }

    SECTION("Infinity values") {
        // Positive infinity: 0x7C00 (exponent=31, mantissa=0)
        float pos_inf = convertFloat16ToFloat(0x7C00);
        CHECK(std::isinf(pos_inf));
        CHECK(pos_inf > 0);

        // Negative infinity: 0xFC00
        float neg_inf = convertFloat16ToFloat(0xFC00);
        CHECK(std::isinf(neg_inf));
        CHECK(neg_inf < 0);
    }

    SECTION("NaN values") {
        // Various NaN representations (exponent=31, mantissa!=0)
        CHECK(std::isnan(convertFloat16ToFloat(0x7C01)));  // Positive NaN
        CHECK(std::isnan(convertFloat16ToFloat(0x7FFF)));  // Max positive NaN
        CHECK(std::isnan(convertFloat16ToFloat(0xFC01)));  // Negative NaN
        CHECK(std::isnan(convertFloat16ToFloat(0xFFFF)));  // Max negative NaN
    }

    SECTION("Simple normalized values") {
        // 1.0 in float16: sign=0, exponent=15 (bias=15), mantissa=0
        // Binary: 0011110000000000 = 0x3C00
        CHECK(AreFloatsEqual(convertFloat16ToFloat(0x3C00), 1.0f));

        // -1.0 in float16: sign=1, exponent=15, mantissa=0
        // Binary: 1011110000000000 = 0xBC00
        CHECK(AreFloatsEqual(convertFloat16ToFloat(0xBC00), -1.0f));

        // 2.0 in float16: sign=0, exponent=16, mantissa=0
        // Binary: 0100000000000000 = 0x4000
        CHECK(AreFloatsEqual(convertFloat16ToFloat(0x4000), 2.0f));

        // 0.5 in float16: sign=0, exponent=14, mantissa=0
        // Binary: 0011100000000000 = 0x3800
        CHECK(AreFloatsEqual(convertFloat16ToFloat(0x3800), 0.5f));
    }

    SECTION("Fractional values") {
        // 1.5 in float16: sign=0, exponent=15, mantissa=512 (0.5 in mantissa)
        // Binary: 0011110010000000 = 0x3E00
        CHECK(AreFloatsEqual(convertFloat16ToFloat(0x3E00), 1.5f));

        // 1.25 in float16: sign=0, exponent=15, mantissa=256 (0.25 in mantissa)
        // Binary: 0011110001000000 = 0x3D00
        CHECK(AreFloatsEqual(convertFloat16ToFloat(0x3D00), 1.25f));

        // 1.75 in float16: sign=0, exponent=15, mantissa=768 (0.75 in mantissa)
        // Binary: 0011110011000000 = 0x3F00
        CHECK(AreFloatsEqual(convertFloat16ToFloat(0x3F00), 1.75f));
    }

    SECTION("Small normalized values") {
        // Smallest normalized positive number in float16
        // exponent=1, mantissa=0: 2^(1-15) = 2^(-14) ≈ 6.103515625e-5
        CHECK(AreFloatsEqual(convertFloat16ToFloat(0x0400), 6.103515625e-5f, 1e-10f));

        // Largest normalized number in float16
        // exponent=30, mantissa=1023: (1 + 1023/1024) * 2^(30-15) = 65504
        CHECK(AreFloatsEqual(convertFloat16ToFloat(0x7BFF), 65504.0f));
    }

    SECTION("Denormalized values") {
        // Smallest positive denormalized: mantissa=1
        // Value: 1/1024 * 2^(-14) ≈ 5.960464477539063e-8
        CHECK(AreFloatsEqual(convertFloat16ToFloat(0x0001), 5.960464477539063e-8f, 1e-15f));

        // Largest denormalized: exponent=0, mantissa=1023
        // Value: 1023/1024 * 2^(-14) ≈ 6.097555160522461e-5
        CHECK(AreFloatsEqual(convertFloat16ToFloat(0x03FF), 6.097555160522461e-5f, 1e-10f));

        // Middle denormalized value: mantissa=512
        // Value: 512/1024 * 2^(-14) = 0.5 * 2^(-14) ≈ 3.0517578125e-5
        CHECK(AreFloatsEqual(convertFloat16ToFloat(0x0200), 3.0517578125e-5f, 1e-10f));
    }

    SECTION("Specific mathematical values") {
        // π ≈ 3.140625 in float16 (closest representable value)
        // This should be approximately 0x4248
        float pi_fp16 = convertFloat16ToFloat(0x4248);
        CHECK(AreFloatsEqual(pi_fp16, 3.140625f, 1e-6f));

        // e ≈ 2.71875 in float16 (closest representable value)
        // This should be approximately 0x4170
        float e_fp16 = convertFloat16ToFloat(0x4170);
        CHECK(AreFloatsEqual(e_fp16, 2.71875f, 1e-6f));
    }

    SECTION("Edge cases around zero") {
        // Test values very close to zero
        for (uint16_t i = 1; i <= 10; ++i) {
            float positive = convertFloat16ToFloat(i);
            float negative = convertFloat16ToFloat(0x8000 | i);

            CHECK(positive > 0.0f);
            CHECK(negative < 0.0f);
            CHECK(AreFloatsEqual(positive, -negative));
        }
    }

    SECTION("Symmetry test") {
        // Test that positive and negative values are symmetric
        uint16_t test_values[] = {
            0x3C00,  // 1.0
            0x4000,  // 2.0
            0x3E00,  // 1.5
            0x4248,  // ~π
            0x7BFF   // Max normal
        };

        for (uint16_t val : test_values) {
            float positive = convertFloat16ToFloat(val);
            float negative = convertFloat16ToFloat(val | 0x8000);  // Set sign bit

            CHECK(AreFloatsEqual(positive, -negative));
        }
    }

    SECTION("Progressive values") {
        // Test a sequence of increasing values
        float prev = convertFloat16ToFloat(0x0001);  // Start with smallest positive

        for (uint16_t i = 0x0002; i < 0x1000; ++i) {
            float curr = convertFloat16ToFloat(i);
            REQUIRE(curr > prev);  // Should be strictly increasing
            prev = curr;
        }
    }

    SECTION("All the floats") {
        for (uint32_t i = 0; i <= std::numeric_limits<uint16_t>::max(); ++i) {
            // Just check that we do not crash / trigger sanitizer.
            if (convertFloat16ToFloat(i) == 1e25f) {
                std::abort();
            };
        }
    }
}
