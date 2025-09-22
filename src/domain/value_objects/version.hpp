#pragma once

#include <string>

namespace domain::value_objects {

class Version {
    int major;
    int minor;
    int patch;

public:
    Version(int major, int minor, int patch)
        : major(major), minor(minor), patch(patch) {}

    const std::string getStringVersion() const noexcept;

    bool operator==(const Version& other) const noexcept {
        return std::tie(major, minor, patch) == std::tie(other.major, other.minor, other.patch);
    }
};

} // namespace domain::value_objects
