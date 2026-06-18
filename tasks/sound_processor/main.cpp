#include "application.h"

#include <iostream>

int main(int argc, char* argv[]) {
    Application app;
    return app.Start(argc, argv, std::cout, std::cerr);
}
