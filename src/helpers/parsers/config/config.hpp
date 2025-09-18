#pragma once

#include <functional>
#include <optional>
#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include "plugin_model.hpp"

namespace helpers {
namespace config {

class ConfigParser {
    std::vector<PluginModel> pluginsInfo;

public:
    ConfigParser(const std::string& configPath);
    const std::string pluginInfo() const;
    std::optional<std::reference_wrapper<const PluginModel>> getPlugin(const std::string& plugin) const;
    PluginModel getWorkPlugin() const;
};

} // config
} // helpers

#endif // CONFIG_H