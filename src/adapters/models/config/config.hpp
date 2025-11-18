#pragma once

#include "plugin_info.hpp"
#include "version.hpp"
#include <optional>
#include <string>
#include <vector>

using domain::value_objects::Version;
using domain::value_objects::PluginInfo;

namespace adapters::config {

class ConfigParser {
    std::vector<PluginInfo> pluginsInfo;

public:
    explicit ConfigParser(const std::string& configPath);
    std::string pluginInfo() const noexcept;
    std::optional<PluginInfo> getPlugin(const std::string& pluginName);
};

} // namespace adapters::config
