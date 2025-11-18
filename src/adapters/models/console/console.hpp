#pragma once

#include "console_args.hpp"
#include "cxxopts.hpp"
#include <string>
#include <vector>

namespace adapters::console {

struct MissingArgument {
    std::string argument;
    std::string message;
};

class ConsoleParser {
    cxxopts::Options options;
    cxxopts::ParseResult result;

public:
    explicit ConsoleParser(int argc, char** argv);
    bool hasOption(const std::string& opt) const noexcept;
    template<typename T>
    T getOption(const std::string& opt) const { return result[opt].as<T>(); }
    std::vector<MissingArgument> checkRequired() const noexcept;
    std::string help() const noexcept;
    domain::dtos::ConsoleArgs getArgs() const;
};

} // namespace adapters::console
