#pragma once

#include <filesystem>
#include <mutex>
#include <source_location>
#include <string>
#include <string_view>
#include <fstream>

namespace utils::logger {

enum LogLevel { VERBOSE = 0, DEBUG, INFO, WARNING, ERROR, CRITICAL };

class Logger {
    std::ofstream logFile;
    std::string ResolveLevel(LogLevel level);
    LogLevel min_level;
    std::mutex file_mutex;

    void log(LogLevel level, std::string_view msg,
             const std::source_location& location);
 public:
    explicit Logger(const std::filesystem::path& fileName, size_t verboseCount);
    ~Logger() noexcept;

    void verbose(std::string_view msg, const std::source_location& location = std::source_location::current());
    void debug(std::string_view msg, const std::source_location& location = std::source_location::current());
    void info(std::string_view msg, const std::source_location& location = std::source_location::current());
    void warning(std::string_view msg, const std::source_location& location = std::source_location::current());
    void error(std::string_view msg, const std::source_location& location = std::source_location::current());
    void critical(std::string_view msg, const std::source_location& location = std::source_location::current());
};

}  // namespace utils::logger

