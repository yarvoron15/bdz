#include "wav_io.h"

#include <cstdint>
#include <fstream>
#include <stdexcept>
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

template <class T>
T ReadStruct(std::istream& input) {
    T value{};
    input.read(reinterpret_cast<char*>(&value), sizeof(value));
    if (!input) {
        throw std::runtime_error("Unexpected end of WAV file.");
    }
    return value;
}

}  // namespace

Waveform WavReader::Read(const std::string& path) const {
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        throw std::runtime_error("Cannot open input WAV file: " + path);
    }

    const RiffHeader riff = ReadStruct<RiffHeader>(input);
    if (riff.id != kRiff || riff.wave_id != kWave) {
        throw std::runtime_error("Input file is not a RIFF/WAVE file.");
    }

    bool found_fmt = false;
    bool found_data = false;
    FmtBody fmt{};
    std::vector<int16_t> samples;

    while (input && (!found_fmt || !found_data)) {
        if (input.peek() == std::char_traits<char>::eof()) {
            break;
        }

        const ChunkHeader chunk = ReadStruct<ChunkHeader>(input);
        if (chunk.id == kFmt) {
            if (chunk.size < sizeof(FmtBody)) {
                throw std::runtime_error("Invalid fmt chunk.");
            }
            fmt = ReadStruct<FmtBody>(input);
            if (chunk.size > sizeof(FmtBody)) {
                input.seekg(chunk.size - sizeof(FmtBody), std::ios::cur);
            }
            found_fmt = true;
        } else if (chunk.id == kData) {
            if (chunk.size % sizeof(int16_t) != 0) {
                throw std::runtime_error("Invalid data chunk size.");
            }
            samples.resize(chunk.size / sizeof(int16_t));
            input.read(reinterpret_cast<char*>(samples.data()), static_cast<std::streamsize>(chunk.size));
            if (!input) {
                throw std::runtime_error("Unexpected end of WAV data chunk.");
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
        throw std::runtime_error("WAV file must contain fmt and data chunks.");
    }
    if (fmt.audio_format != 1 || fmt.channels != 1 || fmt.sample_rate != Waveform::kSampleRate ||
        fmt.bits_per_sample != 16 || fmt.block_align != sizeof(int16_t)) {
        throw std::runtime_error("Only mono PCM WAV 44.1kHz 16-bit files are supported.");
    }

    return Waveform(std::move(samples));
}

void WavWriter::Write(const std::string& path, const Waveform& waveform) const {
    std::ofstream output(path, std::ios::binary);
    if (!output) {
        throw std::runtime_error("Cannot open output WAV file: " + path);
    }

    const uint32_t data_size = static_cast<uint32_t>(waveform.size() * sizeof(int16_t));
    const RiffHeader riff{kRiff, 36U + data_size, kWave};
    const ChunkHeader fmt_header{kFmt, 16U};
    const FmtBody fmt_body{1, 1, static_cast<uint32_t>(Waveform::kSampleRate),
                           static_cast<uint32_t>(Waveform::kSampleRate * sizeof(int16_t)),
                           sizeof(int16_t), 16};
    const ChunkHeader data_header{kData, data_size};

    output.write(reinterpret_cast<const char*>(&riff), sizeof(riff));
    output.write(reinterpret_cast<const char*>(&fmt_header), sizeof(fmt_header));
    output.write(reinterpret_cast<const char*>(&fmt_body), sizeof(fmt_body));
    output.write(reinterpret_cast<const char*>(&data_header), sizeof(data_header));
    if (!waveform.empty()) {
        output.write(reinterpret_cast<const char*>(waveform.samples().data()),
                     static_cast<std::streamsize>(data_size));
    }
    if (!output) {
        throw std::runtime_error("Cannot write output WAV file: " + path);
    }
}
