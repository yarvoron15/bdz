#ifndef SOUND_PROCESSOR_PIPELINE_H_
#define SOUND_PROCESSOR_PIPELINE_H_

#include "filters.h"

#include <memory>
#include <vector>

class Pipeline {
public:
    void AddFilter(std::unique_ptr<IFilter> filter);
    void Apply(Waveform* waveform) const;
    size_t Size() const;

private:
    std::vector<std::unique_ptr<IFilter>> filters_;
};

#endif  // SOUND_PROCESSOR_PIPELINE_H_
