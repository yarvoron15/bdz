#include "waveform.h"

#include <algorithm>
#include <cmath>
#include <limits>

Waveform::Waveform(std::vector<int16_t> samples) : samples_(std::move(samples)) {
}

std::vector<int16_t>& Waveform::samples() {
    return samples_;
}

const std::vector<int16_t>& Waveform::samples() const {
    return samples_;
}

size_t Waveform::size() const {
    return samples_.size();
}

bool Waveform::empty() const {
    return samples_.empty();
}

void Waveform::clear() {
    samples_.clear();
}

size_t Waveform::MillisecondsToSamples(double milliseconds) {
    if (milliseconds <= 0) {
        return 0;
    }
    return static_cast<size_t>(std::llround(milliseconds * kSampleRate / 1000.0));
}

size_t Waveform::SecondsToSamples(double seconds) {
    if (seconds <= 0) {
        return 0;
    }
    return static_cast<size_t>(std::llround(seconds * kSampleRate));
}

int16_t Waveform::ClampSample(double value) {
    if (value > static_cast<double>(kMaxSample)) {
        return kMaxSample;
    }
    if (value < static_cast<double>(kMinSample)) {
        return kMinSample;
    }
    return static_cast<int16_t>(std::lrint(value));
}
