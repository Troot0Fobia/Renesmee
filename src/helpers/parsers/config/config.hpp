#pragma once

#include <optional>
#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include "plugin_info_model.hpp"

namespace helpers {
namespace config {

class ConfigParser {
    std::vector<PluginInfoModel> pluginsInfo;

public:
    ConfigParser(const std::string& configPath);
    const std::string pluginInfo() const;
    std::optional<PluginInfoModel> getPlugin(const std::string& plugin);
    PluginInfoModel getWorkPlugin() const;
};

} // config
} // helpers

#endif // CONFIG_H