#include "args_parser.h"

namespace {

bool IsOptionToken(const char* token) {
    if (token == nullptr) {
        return false;
    }
    const std::string value = token;
    return value == "-i" || value == "-o" || value == "-f";
}

}  // namespace

ArgsParser::Result ArgsParser::Parse(int argc, char* argv[]) {
    input_path_.clear();
    output_path_.clear();
    filters_.clear();

    if (argc <= 1) {
        return Result::no_args;
    }

    int i = 1;
    while (i < argc) {
        const std::string token = argv[i];
        if (token == "-i") {
            if (i + 1 >= argc || IsOptionToken(argv[i + 1])) {
                return Result::bad_args;
            }
            input_path_ = argv[i + 1];
            i += 2;
        } else if (token == "-o") {
            if (i + 1 >= argc || IsOptionToken(argv[i + 1])) {
                return Result::bad_args;
            }
            output_path_ = argv[i + 1];
            i += 2;
        } else if (token == "-f") {
            if (i + 1 >= argc || IsOptionToken(argv[i + 1])) {
                return Result::bad_args;
            }
            FilterDescriptor descriptor;
            descriptor.name = argv[i + 1];
            i += 2;
            while (i < argc && !IsOptionToken(argv[i])) {
                descriptor.params.push_back(argv[i]);
                ++i;
            }
            filters_.push_back(std::move(descriptor));
        } else {
            return Result::bad_args;
        }
    }

    return Result::ok;
}

const std::string& ArgsParser::InputPath() const {
    return input_path_;
}

const std::string& ArgsParser::OutputPath() const {
    return output_path_;
}

const std::vector<FilterDescriptor>& ArgsParser::Filters() const {
    return filters_;
}
