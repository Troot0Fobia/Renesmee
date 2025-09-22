#pragma once

#include "version.hpp"

namespace domain::value_objects {

struct PluginInfo {
    std::string name;
    std::string description;
    std::string path;
    Version version;
};

} // namespace domain::value_objects
