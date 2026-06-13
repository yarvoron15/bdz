#include <catch2/catch_shim.hpp>

#include "varint.h"

TEST_CASE("DecodeVarint - Single byte values") {
    SECTION("Zero") {
        uint8_t data[] = {0x00};
        uint64_t result;
        size_t consumed = decodeVarint(data, sizeof(data), result);
        CHECK(consumed == 1);
        CHECK(result == 0);
    }

    SECTION("Small positive numbers") {
        uint8_t data1[] = {0x01};
        uint64_t result;
        size_t consumed = decodeVarint(data1, sizeof(data1), result);
        CHECK(consumed == 1);
        CHECK(result == 1);

        uint8_t data42[] = {0x2A};
        consumed = decodeVarint(data42, sizeof(data42), result);
        CHECK(consumed == 1);
        CHECK(result == 42);

        uint8_t data127[] = {0x7F};
        consumed = decodeVarint(data127, sizeof(data127), result);
        CHECK(consumed == 1);
        CHECK(result == 127);
    }
}

TEST_CASE("DecodeVarint - Multi-byte values") {
    SECTION("128 - first multi-byte value") {
        // 128 = first number requiring 2 bytes
        uint8_t data[] = {0x80, 0x01};
        uint64_t result;
        size_t consumed = decodeVarint(data, sizeof(data), result);
        CHECK(consumed == 2);
        CHECK(result == 128);
    }

    SECTION("150 - classic varint example") {
        // 150 = 0x96, 0x01 in varint
        uint8_t data[] = {0x96, 0x01};
        uint64_t result;
        size_t consumed = decodeVarint(data, sizeof(data), result);
        CHECK(consumed == 2);
        CHECK(result == 150);
    }

    SECTION("16383 - largest 2-byte value") {
        // 16383 = largest value encodable in 2 bytes
        uint8_t data[] = {0xFF, 0x7F};
        uint64_t result;
        size_t consumed = decodeVarint(data, sizeof(data), result);
        CHECK(consumed == 2);
        CHECK(result == 16383);
    }

    SECTION("16384 - first 3-byte value") {
        // 16384 = first number requiring 3 bytes
        uint8_t data[] = {0x80, 0x80, 0x01};
        uint64_t result;
        size_t consumed = decodeVarint(data, sizeof(data), result);
        CHECK(consumed == 3);
        CHECK(result == 16384);
    }
}

TEST_CASE("DecodeVarint - boundary values") {
    SECTION("Powers of 2") {
        struct TestCase {
            std::vector<uint8_t> encoded;
            uint64_t expected;
        };

        std::vector<TestCase> cases = {
            {{0x80, 0x01}, 128},                    // 2^7
            {{0x80, 0x02}, 256},                    // 2^8
            {{0x80, 0x80, 0x01}, 16384},           // 2^14
            {{0x80, 0x80, 0x02}, 32768},           // 2^15
            {{0x80, 0x80, 0x80, 0x01}, 2097152},   // 2^21
        };

        for (const auto& test_case : cases) {
            uint64_t result;
            size_t consumed = decodeVarint(test_case.encoded.data(), test_case.encoded.size(), result);
            CHECK(consumed == test_case.encoded.size());
            CHECK(result == test_case.expected);
        }
    }
}

TEST_CASE("DecodeVarint - Maximum values") {
    SECTION("Maximum uint64_t value") {
        // UINT64_MAX encoded as varint (10 bytes)
        uint8_t data[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01};
        uint64_t result;
        size_t consumed = decodeVarint(data, sizeof(data), result);
        CHECK(consumed == 10);
        CHECK(result == UINT64_MAX);
    }

    SECTION("Large realistic value") {
        // 1 billion (common in real applications)
        uint8_t data[] = {0x80, 0x94, 0xEB, 0xDC, 0x03};
        uint64_t result;
        size_t consumed = decodeVarint(data, sizeof(data), result);
        CHECK(consumed == 5);
        CHECK(result == 1000000000ULL);
    }
}

TEST_CASE("DecodeVarint - Error cases") {
    SECTION("Empty buffer") {
        uint64_t result;
        size_t consumed = decodeVarint(nullptr, 0, result);
        CHECK(consumed == 0);
    }

    SECTION("Incomplete varint - missing continuation") {
        uint8_t data[] = {0x96};  // Continuation bit set, but no next byte
        uint64_t result;
        size_t consumed = decodeVarint(data, sizeof(data), result);
        CHECK(consumed == 0);
    }

    SECTION("Too long varint - limit exceeded") {
        // More than 10 bytes - our maximum
        uint8_t data[] = {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80};
        uint64_t result = 42;
        size_t consumed = decodeVarint(data, sizeof(data), result);
        CHECK(consumed == 0);
        CHECK(result == 42);
    }

    SECTION("No terminating byte within available data") {
        uint8_t data[] = {0x80, 0x80, 0x80};  // All have continuation bit
        uint64_t result = 42;
        size_t consumed = decodeVarint(data, sizeof(data), result);
        CHECK(consumed == 0);
        CHECK(result == 42);
    }

    SECTION("Invalid varint - 10th byte >1") {
        uint8_t data[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x77};
        uint64_t result = 42;
        size_t consumed = decodeVarint(data, sizeof(data), result);
        CHECK(consumed == 0);
        CHECK(result == 42);
    }
}

TEST_CASE("DecodeVarint - Stream parsing scenarios") {
    SECTION("Varint at start of larger buffer") {
        // Real-world scenario: parsing from a larger stream
        uint8_t data[] = {0x96, 0x01, 0xFF, 0xFF, 0xFF};  // Extra bytes after varint
        uint64_t result;
        size_t consumed = decodeVarint(data, sizeof(data), result);
        CHECK(consumed == 2);  // Should only consume the varint
        CHECK(result == 150);
    }

    SECTION("Multiple varints in sequence") {
        // Simulate parsing multiple varints from a stream
        uint8_t data[] = {0x96, 0x01, 0x80, 0x01, 0x7F};  // 150, 128, 127
        uint64_t result;

        // Parse first varint
        size_t consumed = decodeVarint(data, sizeof(data), result);
        CHECK(consumed == 2);
        CHECK(result == 150);

        // Parse second varint
        consumed = decodeVarint(data + 2, sizeof(data) - 2, result);
        CHECK(consumed == 2);
        CHECK(result == 128);

        // Parse third varint
        consumed = decodeVarint(data + 4, sizeof(data) - 4, result);
        CHECK(consumed == 1);
        CHECK(result == 127);
    }

    SECTION("Exact buffer size matching") {
        uint8_t data[] = {0x96, 0x01};
        uint64_t result;

        // Buffer exactly matches varint length
        size_t consumed = decodeVarint(data, 2, result);
        CHECK(consumed == 2);
        CHECK(result == 150);

        // Buffer too short by 1 byte
        result = 42;
        consumed = decodeVarint(data, 1, result);
        CHECK(consumed == 0);
        CHECK(result == 42);
    }
}

TEST_CASE("DecodeVarint - Edge cases and robustness") {
    SECTION("All single-byte values") {
        // Test all valid single-byte values (0-127)
        for (uint64_t i = 0; i <= 127; ++i) {
            uint8_t data[] = {static_cast<uint8_t>(i)};
            uint64_t result;
            size_t consumed = decodeVarint(data, sizeof(data), result);
            CHECK(consumed == 1);
            CHECK(result == i);
        }
    }

    SECTION("Byte boundary transitions") {
        struct TestCase {
            uint64_t value;
            std::vector<uint8_t> expected_encoding;
        };

        std::vector<TestCase> boundary_cases = {
            {127, {0x7F}},                    // Largest 1-byte
            {128, {0x80, 0x01}},             // Smallest 2-byte
            {16383, {0xFF, 0x7F}},           // Largest 2-byte
            {16384, {0x80, 0x80, 0x01}},     // Smallest 3-byte
        };

        for (const auto& test_case : boundary_cases) {
            uint64_t result;
            size_t consumed = decodeVarint(test_case.expected_encoding.data(),
                                         test_case.expected_encoding.size(), result);
            CHECK(consumed == test_case.expected_encoding.size());
            CHECK(result == test_case.value);
        }
    }
}
