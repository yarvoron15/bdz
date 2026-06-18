#include "pipeline.h"

#include <stdexcept>

void Pipeline::AddFilter(std::unique_ptr<IFilter> filter) {
    if (filter == nullptr) {
        throw std::runtime_error("Cannot add null filter to pipeline.");
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
