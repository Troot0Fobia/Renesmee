#pragma once

#include "plugin_info.hpp"

#include <optional>
#include <vector>

using namespace domain::value_objects;

namespace adapters::config {

class ConfigParser {
    std::vector<PluginInfo> pluginsInfo;

public:
    explicit ConfigParser(const std::string& configPath);
    std::string pluginInfo() const noexcept;
    std::optional<PluginInfo> getPlugin(const std::string& pluginName);
};

} // namespace adapters::config
