#include <catch2/catch_shim.hpp>

#include "application.h"
#include "args_parser.h"
#include "converter.h"
#include "filters.h"
#include "waveform.h"
#include "wav_io.h"

#include <cstdio>
#include <sstream>

namespace {

std::string MakeTempPath(const char* name) {
    return std::string("C:\\Users\\yarvo\\AppData\\Local\\Temp\\opencode\\") + name;
}

}  // namespace

TEST_CASE("ArgsParser parses CLI") {
    char app[] = "sound_processor";
    char in[] = "-i";
    char in_file[] = "input.wav";
    char out[] = "-o";
    char out_file[] = "output.wav";
    char filter[] = "-f";
    char ampl[] = "ampl";
    char factor[] = "0.5";
    char normalize[] = "normalize";
    char* argv[] = {app, in, in_file, out, out_file, filter, ampl, factor, filter, normalize};

    ArgsParser parser;
    REQUIRE(parser.Parse(10, argv) == ArgsParser::Result::ok);
    REQUIRE(parser.InputPath() == "input.wav");
    REQUIRE(parser.OutputPath() == "output.wav");
    REQUIRE(parser.Filters().size() == 2);
    REQUIRE(parser.Filters()[0].name == "ampl");
    REQUIRE(parser.Filters()[1].name == "normalize");
}

TEST_CASE("ArgsParser accepts negative numeric token as parameter") {
    char app[] = "sound_processor";
    char filter[] = "-f";
    char ampl[] = "ampl";
    char negative[] = "-0.5";
    char* argv[] = {app, filter, ampl, negative};

    ArgsParser parser;
    REQUIRE(parser.Parse(4, argv) == ArgsParser::Result::ok);
    REQUIRE(parser.Filters().size() == 1);
    REQUIRE(parser.Filters()[0].params == std::vector<std::string>{"-0.5"});
}

TEST_CASE("Pipeline applies ampl and normalize") {
    CommandLineArgsToPipelineConverter converter;
    const std::vector<FilterDescriptor> descriptors = {
        {"ampl", {"0.5"}},
        {"normalize", {"1"}},
    };

    Waveform waveform({1000, -2000, 500});
    const Pipeline pipeline = converter.CreatePipeline(descriptors);
    pipeline.Apply(&waveform);

    REQUIRE(waveform.size() == 3);
    REQUIRE(waveform.samples()[1] == -32767);
}

TEST_CASE("Silence filter inserts zeros into waveform") {
    Waveform waveform({1, 2, 3, 4});
    SilenceFilter filter("ms", 0.0, 0.0);

    filter.Apply(&waveform);

    REQUIRE(waveform.size() == 5);
    REQUIRE(waveform.samples()[0] == 0);
    REQUIRE(waveform.samples()[1] == 1);
}

TEST_CASE("Timestretch keeps boundary samples") {
    Waveform waveform({0, 1000, 2000});
    TimeStretchFilter filter(2.0);

    filter.Apply(&waveform);

    REQUIRE(waveform.size() == 6);
    REQUIRE(waveform.samples().front() == 0);
    REQUIRE(waveform.samples().back() == 2000);
}

TEST_CASE("Lowpass smooths impulse") {
    Waveform waveform({0, 0, 3000, 0, 0});
    LowpassFilter filter(3);

    filter.Apply(&waveform);

    REQUIRE(waveform.samples()[1] > 0);
    REQUIRE(waveform.samples()[2] < 3000);
    REQUIRE(waveform.samples()[3] > 0);
}

TEST_CASE("Generator creates non-empty waveform") {
    CommandLineArgsToPipelineConverter converter;
    const std::vector<FilterDescriptor> descriptors = {
        {"generator", {"sin", "440", "100"}},
    };

    Waveform waveform;
    const Pipeline pipeline = converter.CreatePipeline(descriptors);
    pipeline.Apply(&waveform);

    REQUIRE(!waveform.empty());
    REQUIRE(waveform.size() == Waveform::MillisecondsToSamples(100));
}

TEST_CASE("Wav writer and reader preserve samples") {
    const std::string path = MakeTempPath("sound_processor_roundtrip.wav");
    const Waveform original({0, 100, -100, 32767, -32768, 42});

    WavWriter writer;
    WavReader reader;
    writer.Write(path, original);

    const Waveform restored = reader.Read(path);
    REQUIRE(restored.samples() == original.samples());

    std::remove(path.c_str());
}

TEST_CASE("Application prints help without args") {
    char app_name[] = "sound_processor";
    char* argv[] = {app_name};
    std::ostringstream out;
    std::ostringstream err;

    Application app;
    REQUIRE(app.Start(1, argv, out, err) == 0);
    REQUIRE(out.str().find("Usage:") != std::string::npos);
}

TEST_CASE("Application processes WAV end-to-end") {
    const std::string input_path = MakeTempPath("sound_processor_in.wav");
    const std::string output_path = MakeTempPath("sound_processor_out.wav");
    WavWriter writer;
    WavReader reader;
    writer.Write(input_path, Waveform({1000, -1000, 2000, -2000}));

    char app_name[] = "sound_processor";
    char in_flag[] = "-i";
    char in_value[] = "C:\\Users\\yarvo\\AppData\\Local\\Temp\\opencode\\sound_processor_in.wav";
    char out_flag[] = "-o";
    char out_value[] = "C:\\Users\\yarvo\\AppData\\Local\\Temp\\opencode\\sound_processor_out.wav";
    char filter_flag[] = "-f";
    char ampl[] = "ampl";
    char factor[] = "0.5";
    char* argv[] = {app_name, in_flag, in_value, out_flag, out_value, filter_flag, ampl, factor};

    std::ostringstream out;
    std::ostringstream err;
    Application app;
    REQUIRE(app.Start(8, argv, out, err) == 0);

    const Waveform result = reader.Read(output_path);
    REQUIRE(result.samples() == std::vector<int16_t>{500, -500, 1000, -1000});

    std::remove(input_path.c_str());
    std::remove(output_path.c_str());
}

TEST_CASE("Application reports bad filter") {
    char app_name[] = "sound_processor";
    char filter_flag[] = "-f";
    char bad[] = "unknown";
    char* argv[] = {app_name, filter_flag, bad};

    std::ostringstream out;
    std::ostringstream err;
    Application app;
    REQUIRE(app.Start(3, argv, out, err) == 2);
    REQUIRE(err.str().find("Unknown filter") != std::string::npos);
}
