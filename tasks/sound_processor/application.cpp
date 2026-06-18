#include "application.h"

#include <exception>
#include <ostream>

int Application::Start(int argc, char* argv[], std::ostream& out, std::ostream& err) const {
    try {
        ArgsParser parser;
        const ArgsParser::Result parse_result = parser.Parse(argc, argv);
        if (parse_result == ArgsParser::Result::no_args) {
            PrintHelp(out);
            return 0;
        }
        if (parse_result == ArgsParser::Result::bad_args) {
            err << "Invalid command line arguments.\n";
            PrintHelp(err);
            return 1;
        }

        Waveform waveform;
        if (!parser.InputPath().empty()) {
            waveform = reader_.Read(parser.InputPath());
        }

        const Pipeline pipeline = converter_.CreatePipeline(parser.Filters());
        pipeline.Apply(&waveform);

        if (!parser.OutputPath().empty()) {
            writer_.Write(parser.OutputPath(), waveform);
        }
        return 0;
    } catch (const std::exception& ex) {
        err << ex.what() << '\n';
        return 2;
    } catch (...) {
        err << "Unknown error.\n";
        return 3;
    }
}

void Application::PrintHelp(std::ostream& out) {
    out << "Usage: sound_processor [-i input.wav] [-o output.wav] "
           "[-f filter_name [filter_params...]]...\n"
           "Filters:\n"
           "  ampl factor\n"
           "  normalize [peak]\n"
           "  silence unit start end\n"
           "  timestretch factor\n"
           "  lowpass window_size\n"
           "  generator sin frequency_hz duration_ms\n"
           "  generator am amplitude carrier_hz modulation_hz depth duration_ms\n"
           "  generator fm amplitude carrier_hz modulation_hz deviation_hz duration_ms\n";
}
