#include "pipeline.h"

#include "errors.h"

void Pipeline::AddFilter(std::unique_ptr<IFilter> filter) {
    if (filter == nullptr) {
        throw SoundProcessorError("Cannot add null filter to pipeline.");
    }
    filters_.push_back(std::move(filter));
}

void Pipeline::Apply(Waveform* waveform) const {
    for (const auto& filter : filters_) {
        filter->Apply(waveform);
    }
}

size_t Pipeline::Size() const {
    return filters_.size();
}
