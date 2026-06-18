#ifndef SOUND_PROCESSOR_WAV_IO_H_
#define SOUND_PROCESSOR_WAV_IO_H_

#include "waveform.h"

#include <iosfwd>
#include <string>

class WavReader {
public:
    [[nodiscard]] Waveform Read(const std::string& path) const;
};

class WavWriter {
public:
    void Write(const std::string& path, const Waveform& waveform) const;
};

#endif  // SOUND_PROCESSOR_WAV_IO_H_
