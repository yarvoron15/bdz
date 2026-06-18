#ifndef SOUND_PROCESSOR_APPLICATION_H_
#define SOUND_PROCESSOR_APPLICATION_H_

#include "args_parser.h"
#include "converter.h"
#include "wav_io.h"

#include <iosfwd>

class Application {
public:
    int Start(int argc, char* argv[], std::ostream& out, std::ostream& err) const;

private:
    static void PrintHelp(std::ostream& out);

    CommandLineArgsToPipelineConverter converter_;
    WavReader reader_;
    WavWriter writer_;
};

#endif  // SOUND_PROCESSOR_APPLICATION_H_
