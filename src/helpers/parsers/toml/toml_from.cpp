#include "toml_from.hpp"
#include <regex>

bool parseVersion(const std::string& versionString, Version& version) {
    std::smatch match;
    int t;
    if (std::regex_match(versionString, match, std::regex(R"(^(\d+)\.(\d+)\.(\d+)$)"))) {
        t = std::stoi(match[1].str());
        if (!isUShort(t)) return false;
            // throw std::out_of_range("Major version is out of range");
        version.major = t;

        t = std::stoi(match[2].str());
        if (!isUShort(t)) return false;
            // throw std::out_of_range("Minor version is out of range");
        version.minor = t;

        t = std::stoi(match[3].str());
        if (!isUShort(t)) return false;
            // throw std::out_of_range("Patch version is out of range");
        version.patch = t;

        return true;
    }
    return false;
}