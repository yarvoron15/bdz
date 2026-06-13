#ifndef VARINT_H_
#define VARINT_H_


#include <cstddef>
#include <cstdint>

// Decode a varint from a byte sequence and return the number of bytes consumed.
// Sets 'result' to the decoded value, if the varint is valid.
// Returns 0 iff the varint is invalid or the buffer is too short.
size_t decodeVarint(const uint8_t* data, size_t size, uint64_t& result);


#endif // VARINT_H_
