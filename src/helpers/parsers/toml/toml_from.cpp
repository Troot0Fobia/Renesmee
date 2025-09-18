#include "toml_from.hpp"
#include <regex>

bool parseVersion(const std::string& versionString, Version& version) {
    std::smatch match;
    int t;
    if (std::regex_match(versionString, match, std::regex(R"(^(\d+)\.(\d+)\.(\d+)$)"))) {
        t = std::stoi(match[1].str());
        if (!isUShort(t))
            return false;
        
        version.major = t;

        t = std::stoi(match[2].str());
        if (!isUShort(t))
            return false;
        
        version.minor = t;

        t = std::stoi(match[3].str());
        if (!isUShort(t))
            return false;
        
        version.patch = t;

        return true;
    }
    return false;
}