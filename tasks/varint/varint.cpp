#include "varint.h"

size_t decodeVarint(const uint8_t* data, size_t size, uint64_t& result)
{
    if (data == nullptr || size == 0) {
        return 0;
    }

    uint64_t value = 0;
    for (size_t i = 0; i < size && i < 10; ++i) {
        const uint8_t byte = data[i];
        const uint64_t payload = byte & 0x7F;

        if (i == 9 && (byte & 0x80) != 0) {
            return 0;
        }
        if (i == 9 && payload > 1) {
            return 0;
        }

        value |= payload << (7 * i);
        if ((byte & 0x80) == 0) {
            result = value;
            return i + 1;
        }
    }

    return 0;
}
