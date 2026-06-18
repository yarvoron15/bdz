#ifndef SOUND_PROCESSOR_ERRORS_H_
#define SOUND_PROCESSOR_ERRORS_H_

#include <stdexcept>
#include <string>

class SoundProcessorError : public std::runtime_error {
public:
    explicit SoundProcessorError(const std::string& message) : std::runtime_error(message) {
    }
};

class WavFormatError : public SoundProcessorError {
public:
    explicit WavFormatError(const std::string& message) : SoundProcessorError(message) {
    }
};

class CliParseError : public SoundProcessorError {
public:
    explicit CliParseError(const std::string& message) : SoundProcessorError(message) {
    }
};

class FilterParseError : public SoundProcessorError {
public:
    explicit FilterParseError(const std::string& message) : SoundProcessorError(message) {
    }
};

#endif  // SOUND_PROCESSOR_ERRORS_H_
