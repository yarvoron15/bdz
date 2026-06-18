#include "filters.h"

#include "errors.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <numeric>
#include <vector>

namespace {

constexpr double kPi = 3.14159265358979323846;

size_t ToSamples(const std::string& unit, double value) {
    if (unit == "sec") {
        return Waveform::SecondsToSamples(value);
    }
    if (unit == "ms") {
        return Waveform::MillisecondsToSamples(value);
    }
    throw FilterParseError("Unknown time unit: " + unit);
}

std::vector<int16_t> GenerateBuffer(size_t count) {
    return std::vector<int16_t>(count);
}

double Interpolate(int16_t left, int16_t right, double frac) {
    return static_cast<double>(left) * (1.0 - frac) + static_cast<double>(right) * frac;
}

}  // namespace

AmplFilter::AmplFilter(double factor) : factor_(factor) {
    if (factor < 0) {
        throw FilterParseError("Ampl factor must be non-negative.");
    }
}

void AmplFilter::Apply(Waveform* waveform) const {
    if (waveform == nullptr) {
        throw SoundProcessorError("Ampl filter received null waveform.");
    }
    for (int16_t& sample : waveform->samples()) {
        sample = Waveform::ClampSample(static_cast<double>(sample) * factor_);
    }
}

NormalizeFilter::NormalizeFilter(double peak) : peak_(peak) {
    if (peak < 0 || peak > 1) {
        throw FilterParseError("Normalize peak must be in [0, 1].");
    }
}

void NormalizeFilter::Apply(Waveform* waveform) const {
    if (waveform == nullptr) {
        throw SoundProcessorError("Normalize filter received null waveform.");
    }
    if (waveform->empty()) {
        return;
    }

    int current_peak = 0;
    for (int16_t sample : waveform->samples()) {
        current_peak = std::max(current_peak, std::abs(static_cast<int>(sample)));
    }
    if (current_peak == 0) {
        return;
    }

    const double scale = peak_ * Waveform::kMaxSample / current_peak;
    for (int16_t& sample : waveform->samples()) {
        sample = Waveform::ClampSample(static_cast<double>(sample) * scale);
    }
}

SilenceFilter::SilenceFilter(std::string unit, double start, double end)
    : unit_(std::move(unit)), start_(start), end_(end) {
    if (start < 0 || end < start) {
        throw FilterParseError("Silence interval is invalid.");
    }
}

void SilenceFilter::Apply(Waveform* waveform) const {
    if (waveform == nullptr) {
        throw SoundProcessorError("Silence filter received null waveform.");
    }
    const size_t insert_at = std::min(ToSamples(unit_, start_), waveform->size());
    const size_t start_sample = ToSamples(unit_, start_);
    const size_t end_sample = ToSamples(unit_, end_);
    const size_t silence_size = end_sample >= start_sample ? (end_sample - start_sample + 1) : 0;
    waveform->samples().insert(waveform->samples().begin() + static_cast<std::ptrdiff_t>(insert_at),
                               silence_size, 0);
}

TimeStretchFilter::TimeStretchFilter(double factor) : factor_(factor) {
    if (factor <= 0) {
        throw FilterParseError("Timestretch factor must be positive.");
    }
}

void TimeStretchFilter::Apply(Waveform* waveform) const {
    if (waveform == nullptr) {
        throw SoundProcessorError("Timestretch filter received null waveform.");
    }
    if (waveform->empty()) {
        return;
    }

    const std::vector<int16_t> input = waveform->samples();
    const size_t new_size = static_cast<size_t>(std::llround(input.size() * factor_));
    std::vector<int16_t> output;
    output.reserve(new_size);
    for (size_t i = 0; i < new_size; ++i) {
        const double pos = i / factor_;
        const size_t left = static_cast<size_t>(pos);
        const double frac = pos - left;
        if (left + 1 >= input.size()) {
            output.push_back(input.back());
        } else {
            output.push_back(Waveform::ClampSample(Interpolate(input[left], input[left + 1], frac)));
        }
    }
    waveform->samples() = std::move(output);
}

LowpassFilter::LowpassFilter(int window_size) : window_size_(window_size) {
    if (window_size < 1 || window_size % 2 == 0) {
        throw FilterParseError("Lowpass window size must be a positive odd integer.");
    }
}

void LowpassFilter::Apply(Waveform* waveform) const {
    if (waveform == nullptr) {
        throw SoundProcessorError("Lowpass filter received null waveform.");
    }
    if (waveform->empty()) {
        return;
    }

    const std::vector<int16_t> input = waveform->samples();
    std::vector<int16_t> output(input.size());
    const int radius = window_size_ / 2;
    for (size_t i = 0; i < input.size(); ++i) {
        double sum = 0;
        for (int offset = -radius; offset <= radius; ++offset) {
            long long index = static_cast<long long>(i) + offset;
            if (index < 0) {
                index = 0;
            }
            if (index >= static_cast<long long>(input.size())) {
                index = static_cast<long long>(input.size()) - 1;
            }
            sum += input[static_cast<size_t>(index)];
        }
        output[i] = Waveform::ClampSample(sum / window_size_);
    }
    waveform->samples() = std::move(output);
}

SinGeneratorFilter::SinGeneratorFilter(double frequency_hz, double duration_ms)
    : frequency_hz_(frequency_hz), duration_ms_(duration_ms) {
    if (frequency_hz < 0 || duration_ms < 0) {
        throw FilterParseError("Sin generator parameters must be non-negative.");
    }
}

void SinGeneratorFilter::Apply(Waveform* waveform) const {
    if (waveform == nullptr) {
        throw SoundProcessorError("Sin generator received null waveform.");
    }
    const size_t count = Waveform::MillisecondsToSamples(duration_ms_);
    waveform->samples() = GenerateBuffer(count);
    for (size_t i = 0; i < count; ++i) {
        const double t = static_cast<double>(i) / Waveform::kSampleRate;
        waveform->samples()[i] = Waveform::ClampSample(
            Waveform::kMaxSample * std::sin(2.0 * kPi * frequency_hz_ * t));
    }
}

AmGeneratorFilter::AmGeneratorFilter(double amplitude, double carrier_hz, double modulation_hz,
                                     double depth, double duration_ms)
    : amplitude_(amplitude),
      carrier_hz_(carrier_hz),
      modulation_hz_(modulation_hz),
      depth_(depth),
      duration_ms_(duration_ms) {
    if (amplitude < 0 || amplitude > 1 || carrier_hz < 0 || modulation_hz < 0 || depth < 0 ||
        depth > 1 || duration_ms < 0) {
        throw FilterParseError("AM generator parameters are invalid.");
    }
}

void AmGeneratorFilter::Apply(Waveform* waveform) const {
    if (waveform == nullptr) {
        throw SoundProcessorError("AM generator received null waveform.");
    }
    const size_t count = Waveform::MillisecondsToSamples(duration_ms_);
    waveform->samples() = GenerateBuffer(count);
    for (size_t i = 0; i < count; ++i) {
        const double t = static_cast<double>(i) / Waveform::kSampleRate;
        const double envelope = 1.0 + depth_ * std::sin(2.0 * kPi * modulation_hz_ * t);
        const double carrier = std::sin(2.0 * kPi * carrier_hz_ * t);
        waveform->samples()[i] =
            Waveform::ClampSample(amplitude_ * Waveform::kMaxSample * envelope * carrier);
    }
}

FmGeneratorFilter::FmGeneratorFilter(double amplitude, double carrier_hz, double modulation_hz,
                                     double deviation_hz, double duration_ms)
    : amplitude_(amplitude),
      carrier_hz_(carrier_hz),
      modulation_hz_(modulation_hz),
      deviation_hz_(deviation_hz),
      duration_ms_(duration_ms) {
    if (amplitude < 0 || amplitude > 1 || carrier_hz < 0 || modulation_hz <= 0 || deviation_hz < 0 ||
        duration_ms < 0) {
        throw FilterParseError("FM generator parameters are invalid.");
    }
}

void FmGeneratorFilter::Apply(Waveform* waveform) const {
    if (waveform == nullptr) {
        throw SoundProcessorError("FM generator received null waveform.");
    }
    const size_t count = Waveform::MillisecondsToSamples(duration_ms_);
    waveform->samples() = GenerateBuffer(count);
    for (size_t i = 0; i < count; ++i) {
        const double t = static_cast<double>(i) / Waveform::kSampleRate;
        const double phase = 2.0 * kPi * carrier_hz_ * t +
                             (deviation_hz_ / modulation_hz_) *
                                 std::sin(2.0 * kPi * modulation_hz_ * t);
        waveform->samples()[i] =
            Waveform::ClampSample(amplitude_ * Waveform::kMaxSample * std::sin(phase));
    }
}
