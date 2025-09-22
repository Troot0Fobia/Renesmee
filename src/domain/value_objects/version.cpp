#include "version.hpp"
#include <format>

namespace domain::value_objects {

const std::string Version::getStringVersion() const noexcept {
    return std::format("{}.{}.{}", major, minor, patch);
}

} // namespace domain::value_objects
