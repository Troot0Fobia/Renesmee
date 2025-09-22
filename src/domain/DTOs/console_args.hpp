#pragma once

#include <filesystem>

namespace domain::dtos {

struct ConsoleArgs {
    const std::filesystem::path inputPath;
    const std::filesystem::path loginsPath;
    const std::filesystem::path passwordsPath;
    const std::filesystem::path proxiesPath;
    const std::string outputPath;
    unsigned short threadsCount;
};

} // namespace domain::dtos
