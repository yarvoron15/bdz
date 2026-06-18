#ifndef SOUND_PROCESSOR_FILTERS_H_
#define SOUND_PROCESSOR_FILTERS_H_

#include "waveform.h"

#include <memory>
#include <string>

class IFilter {
public:
    virtual ~IFilter() = default;
    virtual void Apply(Waveform* waveform) const = 0;
};

class AmplFilter : public IFilter {
public:
    explicit AmplFilter(double factor);
    void Apply(Waveform* waveform) const override;

private:
    double factor_;
};

class NormalizeFilter : public IFilter {
public:
    explicit NormalizeFilter(double peak);
    void Apply(Waveform* waveform) const override;

private:
    double peak_;
};

class SilenceFilter : public IFilter {
public:
    SilenceFilter(std::string unit, double start, double end);
    void Apply(Waveform* waveform) const override;

private:
    std::string unit_;
    double start_;
    double end_;
};

class TimeStretchFilter : public IFilter {
public:
    explicit TimeStretchFilter(double factor);
    void Apply(Waveform* waveform) const override;

private:
    double factor_;
};

class LowpassFilter : public IFilter {
public:
    explicit LowpassFilter(int window_size);
    void Apply(Waveform* waveform) const override;

private:
    int window_size_;
};

class SinGeneratorFilter : public IFilter {
public:
    SinGeneratorFilter(double frequency_hz, double duration_ms);
    void Apply(Waveform* waveform) const override;

private:
    double frequency_hz_;
    double duration_ms_;
};

class AmGeneratorFilter : public IFilter {
public:
    AmGeneratorFilter(double amplitude, double carrier_hz, double modulation_hz, double depth,
                      double duration_ms);
    void Apply(Waveform* waveform) const override;

private:
    double amplitude_;
    double carrier_hz_;
    double modulation_hz_;
    double depth_;
    double duration_ms_;
};

class FmGeneratorFilter : public IFilter {
public:
    FmGeneratorFilter(double amplitude, double carrier_hz, double modulation_hz, double deviation_hz,
                      double duration_ms);
    void Apply(Waveform* waveform) const override;

private:
    double amplitude_;
    double carrier_hz_;
    double modulation_hz_;
    double deviation_hz_;
    double duration_ms_;
};

#endif  // SOUND_PROCESSOR_FILTERS_H_
