#include "wav_io.h"

#include "errors.h"

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

namespace {

constexpr uint32_t kRiff = 0x46464952U;
constexpr uint32_t kWave = 0x45564157U;
constexpr uint32_t kFmt = 0x20746d66U;
constexpr uint32_t kData = 0x61746164U;

struct RiffHeader {
    uint32_t id;
    uint32_t size;
    uint32_t wave_id;
};

struct ChunkHeader {
    uint32_t id;
    uint32_t size;
};

struct FmtBody {
    uint16_t audio_format;
    uint16_t channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
};

uint16_t ReadLe16(std::istream& input) {
    unsigned char bytes[2] = {};
    input.read(reinterpret_cast<char*>(bytes), 2);
    if (!input) {
        throw WavFormatError("Unexpected end of WAV file.");
    }
    return static_cast<uint16_t>(bytes[0]) | (static_cast<uint16_t>(bytes[1]) << 8U);
}

uint32_t ReadLe32(std::istream& input) {
    unsigned char bytes[4] = {};
    input.read(reinterpret_cast<char*>(bytes), 4);
    if (!input) {
        throw WavFormatError("Unexpected end of WAV file.");
    }
    return static_cast<uint32_t>(bytes[0]) | (static_cast<uint32_t>(bytes[1]) << 8U) |
           (static_cast<uint32_t>(bytes[2]) << 16U) | (static_cast<uint32_t>(bytes[3]) << 24U);
}

void WriteLe16(std::ostream& output, uint16_t value) {
    const unsigned char bytes[2] = {static_cast<unsigned char>(value & 0xFFU),
                                    static_cast<unsigned char>((value >> 8U) & 0xFFU)};
    output.write(reinterpret_cast<const char*>(bytes), 2);
}

void WriteLe32(std::ostream& output, uint32_t value) {
    const unsigned char bytes[4] = {static_cast<unsigned char>(value & 0xFFU),
                                    static_cast<unsigned char>((value >> 8U) & 0xFFU),
                                    static_cast<unsigned char>((value >> 16U) & 0xFFU),
                                    static_cast<unsigned char>((value >> 24U) & 0xFFU)};
    output.write(reinterpret_cast<const char*>(bytes), 4);
}

RiffHeader ReadRiffHeader(std::istream& input) {
    return {ReadLe32(input), ReadLe32(input), ReadLe32(input)};
}

ChunkHeader ReadChunkHeader(std::istream& input) {
    return {ReadLe32(input), ReadLe32(input)};
}

FmtBody ReadFmtBody(std::istream& input) {
    FmtBody fmt{};
    fmt.audio_format = ReadLe16(input);
    fmt.channels = ReadLe16(input);
    fmt.sample_rate = ReadLe32(input);
    fmt.byte_rate = ReadLe32(input);
    fmt.block_align = ReadLe16(input);
    fmt.bits_per_sample = ReadLe16(input);
    return fmt;
}

}  // namespace

Waveform WavReader::Read(const std::string& path) const {
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        throw WavFormatError("Cannot open input WAV file: " + path);
    }

    const RiffHeader riff = ReadRiffHeader(input);
    if (riff.id != kRiff || riff.wave_id != kWave) {
        throw WavFormatError("Input file is not a RIFF/WAVE file.");
    }

    bool found_fmt = false;
    bool found_data = false;
    FmtBody fmt{};
    std::vector<int16_t> samples;

    while (input && (!found_fmt || !found_data)) {
        if (input.peek() == std::char_traits<char>::eof()) {
            break;
        }

        const ChunkHeader chunk = ReadChunkHeader(input);
        if (chunk.id == kFmt) {
            if (chunk.size < sizeof(FmtBody)) {
                throw WavFormatError("Invalid fmt chunk.");
            }
            fmt = ReadFmtBody(input);
            if (chunk.size > sizeof(FmtBody)) {
                input.seekg(chunk.size - sizeof(FmtBody), std::ios::cur);
            }
            found_fmt = true;
        } else if (chunk.id == kData) {
            if (chunk.size % sizeof(int16_t) != 0) {
                throw WavFormatError("Invalid data chunk size.");
            }
            samples.resize(chunk.size / sizeof(int16_t));
            input.read(reinterpret_cast<char*>(samples.data()), static_cast<std::streamsize>(chunk.size));
            if (!input) {
                throw WavFormatError("Unexpected end of WAV data chunk.");
            }
            found_data = true;
        } else {
            input.seekg(chunk.size, std::ios::cur);
        }

        if (chunk.size % 2U == 1U) {
            input.seekg(1, std::ios::cur);
        }
    }

    if (!found_fmt || !found_data) {
        throw WavFormatError("WAV file must contain fmt and data chunks.");
    }
    if (fmt.audio_format != 1 || fmt.channels != 1 || fmt.sample_rate != Waveform::kSampleRate ||
        fmt.bits_per_sample != 16 || fmt.block_align != sizeof(int16_t)) {
        throw WavFormatError("Only mono PCM WAV 44.1kHz 16-bit files are supported.");
    }

    return Waveform(std::move(samples));
}

void WavWriter::Write(const std::string& path, const Waveform& waveform) const {
    std::ofstream output(path, std::ios::binary);
    if (!output) {
        throw WavFormatError("Cannot open output WAV file: " + path);
    }

    const uint32_t data_size = static_cast<uint32_t>(waveform.size() * sizeof(int16_t));
    WriteLe32(output, kRiff);
    WriteLe32(output, 36U + data_size);
    WriteLe32(output, kWave);
    WriteLe32(output, kFmt);
    WriteLe32(output, 16U);
    WriteLe16(output, 1U);
    WriteLe16(output, 1U);
    WriteLe32(output, static_cast<uint32_t>(Waveform::kSampleRate));
    WriteLe32(output, static_cast<uint32_t>(Waveform::kSampleRate * sizeof(int16_t)));
    WriteLe16(output, static_cast<uint16_t>(sizeof(int16_t)));
    WriteLe16(output, 16U);
    WriteLe32(output, kData);
    WriteLe32(output, data_size);
    if (!waveform.empty()) {
        output.write(reinterpret_cast<const char*>(waveform.samples().data()),
                     static_cast<std::streamsize>(data_size));
    }
    if (!output) {
        throw WavFormatError("Cannot write output WAV file: " + path);
    }
}
