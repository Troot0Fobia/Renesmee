#pragma once

#ifndef CONSOLE_H
#define CONSOLE_H

#include <vector>
#include <string>
#include "cxxopts.hpp"

namespace helpers {
namespace console {

struct MissingArgument {
    std::string argument;
    std::string message;
};

class ConsoleParser {
    cxxopts::Options options;
    cxxopts::ParseResult result;

public:
    bool hasOption(const std::string& opt) const;

    template<typename T>
    T getOption(const std::string& opt) const {
        return result[opt].as<T>();
    }

    ConsoleParser(int argc, char** argv);
    const std::vector<MissingArgument> checkRequired() const;
    const std::string help() const;
};

} // console
} // helpers

#endif // CONSOLE_H