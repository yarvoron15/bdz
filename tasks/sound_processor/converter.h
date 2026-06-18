#ifndef SOUND_PROCESSOR_CONVERTER_H_
#define SOUND_PROCESSOR_CONVERTER_H_

#include "args_parser.h"
#include "pipeline.h"

#include <functional>
#include <map>
#include <memory>
#include <string>

class CommandLineArgsToPipelineConverter {
public:
    using Producer = std::function<std::unique_ptr<IFilter>(const FilterDescriptor&)>;

    CommandLineArgsToPipelineConverter();

    [[nodiscard]] Pipeline CreatePipeline(const std::vector<FilterDescriptor>& descriptors) const;

private:
    std::map<std::string, Producer> producers_;
};

#endif  // SOUND_PROCESSOR_CONVERTER_H_
