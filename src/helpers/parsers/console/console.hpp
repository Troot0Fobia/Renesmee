#pragma once

#ifndef CONSOLE_H
#define CONSOLE_H

#include <cstdlib>
#include <string>
#include "cxxopts.hpp"

namespace helpers {
namespace console {

class ConsoleParser {
    cxxopts::Options options;
    cxxopts::ParseResult result;

public:
    bool hasOption(const std::string& opt) const;

    template<typename T>
    T getOption(const std::string& opt) const;

    ConsoleParser(int argc, char** argv);
    bool checkRequires();
    void printHelp(const std::string& additional_message = "", int exit_code = EXIT_FAILURE);
};

} // console
} // helpers

#endif // CONSOLE_H