#pragma once

#include <filesystem>
#include <string>

namespace utils {

std::filesystem::path resolvePath(const std::string& path_str);
std::filesystem::path createFolder(const std::string& path_str);

} // namespace utils
