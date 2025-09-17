#pragma once

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include "plugin_model.hpp"
#include "toml.hpp"

namespace helpers {
namespace config {

class ConfigParser {
    std::string _configPath;
    toml::basic_value<toml::type_config> data;
    std::vector<PluginModel> pluginsInfo;

public:
    ConfigParser(const std::string& configPath);
    std::vector<PluginModel> parse();
    const std::string pluginInfo() const;

};

bool parseVersion(const std::string& versionString, Version& version);

} // config
} // helpers

#endif // CONFIG_H