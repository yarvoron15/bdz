#ifndef SOUND_PROCESSOR_ARGS_PARSER_H_
#define SOUND_PROCESSOR_ARGS_PARSER_H_

#include <string>
#include <vector>

struct FilterDescriptor {
    std::string name;
    std::vector<std::string> params;
};

class ArgsParser {
public:
    enum class Result {
        ok,
        no_args,
        bad_args,
    };

    [[nodiscard]] Result Parse(int argc, char* argv[]);

    [[nodiscard]] const std::string& InputPath() const;
    [[nodiscard]] const std::string& OutputPath() const;
    [[nodiscard]] const std::vector<FilterDescriptor>& Filters() const;

private:
    std::string input_path_;
    std::string output_path_;
    std::vector<FilterDescriptor> filters_;
};

#endif  // SOUND_PROCESSOR_ARGS_PARSER_H_
