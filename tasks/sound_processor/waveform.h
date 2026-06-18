#ifndef SOUND_PROCESSOR_WAVEFORM_H_
#define SOUND_PROCESSOR_WAVEFORM_H_

#include <cstddef>
#include <cstdint>
#include <vector>

class Waveform {
public:
    static constexpr int kSampleRate = 44100;
    static constexpr int16_t kMaxSample = 32767;
    static constexpr int16_t kMinSample = -32768;

    Waveform() = default;
    explicit Waveform(std::vector<int16_t> samples);

    std::vector<int16_t>& samples();
    const std::vector<int16_t>& samples() const;

    size_t size() const;
    bool empty() const;
    void clear();

    static size_t MillisecondsToSamples(double milliseconds);
    static size_t SecondsToSamples(double seconds);
    static int16_t ClampSample(double value);

private:
    std::vector<int16_t> samples_;
};

#endif  // SOUND_PROCESSOR_WAVEFORM_H_
