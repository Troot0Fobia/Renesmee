#pragma once

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include "plugin_model.hpp"
// #include "toml.hpp"

namespace helpers {
namespace config {

class ConfigParser {
    std::string _configPath;
    std::vector<PluginModel> pluginsInfo;

public:
    ConfigParser(const std::string& configPath);
    const std::string pluginInfo() const;
    bool hasPlugin(const std::string& plugin) const;

};

} // config
} // helpers

#endif // CONFIG_H