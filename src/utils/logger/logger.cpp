#include "logger.hpp"
#include <chrono>
#include <ctime>
#include <filesystem>
#include <format>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <algorithm>

namespace utils::logger {

Logger::Logger(const std::filesystem::path& fileName, size_t verboseCount) {
    logFile.open(fileName, std::ios::app);
    if (!logFile.is_open()) {
        throw std::runtime_error("Failed open log file.\n");
    }
    min_level = static_cast<LogLevel>(
        static_cast<size_t>(WARNING) - std::min(verboseCount,
                                                static_cast<size_t>(3)));
}

Logger::~Logger() noexcept {
    if (logFile.is_open())
        logFile.close();
}

std::string Logger::ResolveLevel(LogLevel level) {
    switch (level) {
        case VERBOSE:
            return "VERBOSE";
        case DEBUG:
            return "DEBUG";
        case INFO:
            return "INFO";
        case WARNING:
            return "WARNING";
        case ERROR:
            return "ERROR";
        case CRITICAL:
            return "CRITICAL";
    }
    return "Unknown level";
}

void Logger::log(LogLevel level, std::string_view msg,
         const std::source_location& location) {
    if (level < min_level) return;

    auto const now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&t), "[%d.%m.%Y %H:%M:%S] ")
       << '[' << ResolveLevel(level) << "] ";
    if (level < DEBUG) {
        ss << std::format("({}@{}:{}) ",
                          location.function_name(),
                          location.file_name(),
                          location.line());
    }
    ss << msg << '\n';

    std::lock_guard<std::mutex> l(file_mutex);
    logFile << ss.str();
    logFile.flush();
}

void Logger::verbose(std::string_view msg, const std::source_location& location) {
    log(LogLevel::VERBOSE, msg, location);
}

void Logger::debug(std::string_view msg, const std::source_location& location) {
    log(LogLevel::DEBUG, msg, location);
}

void Logger::info(std::string_view msg, const std::source_location& location) {
    log(LogLevel::INFO, msg, location);
}

void Logger::warning(std::string_view msg, const std::source_location& location) {
    log(LogLevel::WARNING, msg, location);
}

void Logger::error(std::string_view msg, const std::source_location& location) {
    log(LogLevel::ERROR, msg, location);
}

void Logger::critical(std::string_view msg, const std::source_location& location) {
    log(LogLevel::CRITICAL, msg, location);
}

}  // namespace utils::logger

