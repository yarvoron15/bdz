#include "converter.h"

#include "errors.h"

#include <memory>

namespace {

double ParseDouble(const std::string& value, const std::string& field_name) {
    size_t pos = 0;
    const double parsed = std::stod(value, &pos);
    if (pos != value.size()) {
        throw FilterParseError("Invalid numeric value for " + field_name + ": " + value);
    }
    return parsed;
}

int ParseInt(const std::string& value, const std::string& field_name) {
    size_t pos = 0;
    const int parsed = std::stoi(value, &pos);
    if (pos != value.size()) {
        throw FilterParseError("Invalid integer value for " + field_name + ": " + value);
    }
    return parsed;
}

void EnsureParamsCount(const FilterDescriptor& descriptor, size_t expected) {
    if (descriptor.params.size() != expected) {
        throw FilterParseError("Wrong number of parameters for filter: " + descriptor.name);
    }
}

}  // namespace

CommandLineArgsToPipelineConverter::CommandLineArgsToPipelineConverter() {
    producers_["ampl"] = [](const FilterDescriptor& descriptor) {
        EnsureParamsCount(descriptor, 1);
        return std::make_unique<AmplFilter>(ParseDouble(descriptor.params[0], "factor"));
    };
    producers_["normalize"] = [](const FilterDescriptor& descriptor) {
        if (descriptor.params.empty()) {
            return std::make_unique<NormalizeFilter>(1.0);
        }
        EnsureParamsCount(descriptor, 1);
        return std::make_unique<NormalizeFilter>(ParseDouble(descriptor.params[0], "peak"));
    };
    producers_["silence"] = [](const FilterDescriptor& descriptor) {
        EnsureParamsCount(descriptor, 3);
        return std::make_unique<SilenceFilter>(descriptor.params[0],
                                               ParseDouble(descriptor.params[1], "start"),
                                               ParseDouble(descriptor.params[2], "end"));
    };
    producers_["timestretch"] = [](const FilterDescriptor& descriptor) {
        EnsureParamsCount(descriptor, 1);
        return std::make_unique<TimeStretchFilter>(ParseDouble(descriptor.params[0], "factor"));
    };
    producers_["lowpass"] = [](const FilterDescriptor& descriptor) {
        EnsureParamsCount(descriptor, 1);
        return std::make_unique<LowpassFilter>(ParseInt(descriptor.params[0], "window_size"));
    };
    producers_["generator"] = [](const FilterDescriptor& descriptor) -> std::unique_ptr<IFilter> {
        if (descriptor.params.empty()) {
            throw FilterParseError("Generator kind is required.");
        }
        const std::string& kind = descriptor.params[0];
        if (kind == "sin") {
            if (descriptor.params.size() != 3) {
                throw FilterParseError("Generator sin expects 2 parameters.");
            }
            return std::make_unique<SinGeneratorFilter>(
                ParseDouble(descriptor.params[1], "frequency_hz"),
                ParseDouble(descriptor.params[2], "duration_ms"));
        }
        if (kind == "am") {
            if (descriptor.params.size() != 6) {
                throw FilterParseError("Generator am expects 5 parameters.");
            }
            return std::make_unique<AmGeneratorFilter>(
                ParseDouble(descriptor.params[1], "amplitude"),
                ParseDouble(descriptor.params[2], "carrier_hz"),
                ParseDouble(descriptor.params[3], "modulation_hz"),
                ParseDouble(descriptor.params[4], "depth"),
                ParseDouble(descriptor.params[5], "duration_ms"));
        }
        if (kind == "fm") {
            if (descriptor.params.size() != 6) {
                throw FilterParseError("Generator fm expects 5 parameters.");
            }
            return std::make_unique<FmGeneratorFilter>(
                ParseDouble(descriptor.params[1], "amplitude"),
                ParseDouble(descriptor.params[2], "carrier_hz"),
                ParseDouble(descriptor.params[3], "modulation_hz"),
                ParseDouble(descriptor.params[4], "deviation_hz"),
                ParseDouble(descriptor.params[5], "duration_ms"));
        }
        throw FilterParseError("Unknown generator kind: " + kind);
    };
}

Pipeline CommandLineArgsToPipelineConverter::CreatePipeline(const std::vector<FilterDescriptor>& descriptors) const {
    Pipeline pipeline;
    for (const auto& descriptor : descriptors) {
        auto it = producers_.find(descriptor.name);
        if (it == producers_.end()) {
            throw FilterParseError("Unknown filter: " + descriptor.name);
        }
        pipeline.AddFilter(it->second(descriptor));
    }
    return pipeline;
}
