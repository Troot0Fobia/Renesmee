#pragma once

#include <filesystem>

namespace domain::dtos {

struct ConsoleArgs {
    const std::filesystem::path inputPath;
    const std::filesystem::path loginsPath;
    const std::filesystem::path passwordsPath;
    const std::filesystem::path proxiesPath;
    const std::filesystem::path outputPath;
    size_t verboseCount;
    int threadsCount;
};

}  // namespace domain::dtos
