#include "config.hpp"
#include <algorithm>
#include <filesystem>
#include <format>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>
#include <vector>
#include "plugin_model.hpp"
#include "toml.hpp"
#include "toml11/find.hpp"
#include "toml11/value.hpp"

#include "toml_from.hpp" // need for override method toml::from

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    #define DYNAMIC_LIB_EXTENSION ".dll"
#elif defined(linux) || defined(__linux) || defined(__linux__)
    #define DYNAMIC_LIB_EXTENSION ".so"
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
    #define DYNAMIC_LIB_EXTENSION ".dylib"
#else
    #error Unknown platform
#endif

namespace helpers {
namespace config {

ConfigParser::ConfigParser(const std::string& configPath)
    : _configPath(std::move(configPath))
{
    if (_configPath.empty())
        throw std::invalid_argument("Config path cannot be empty");

    std::error_code err_code;
    std::filesystem::path path(_configPath);

    if (!std::filesystem::exists(path, err_code))
        throw std::filesystem::filesystem_error(
            std::format("Provided path {} does not exist", _configPath),
            err_code
        );

    std::filesystem::path cannonical_path = std::filesystem::canonical(path, err_code);
    
    if (err_code)
        throw std::filesystem::filesystem_error(
            "Provided path {} does not exist", err_code
        );

    auto res = toml::try_parse(cannonical_path);
    
    if (res.is_ok())
        pluginsInfo = toml::find<std::vector<PluginModel>>(std::move(res.unwrap()), "plugins");
    else
        throw std::runtime_error(
            std::format("TOML error parse file {}",
                res.unwrap_err().front().title()
            )
        );
}

const std::string ConfigParser::pluginInfo() const {
    std::string t;
    for (const auto& plugin : pluginsInfo) {
        t += "Plugin name: " + plugin.name + "\n" 
             "Plugin description: " + plugin.description + "\n"
             "Plugin path: " + plugin.path + "\n"
             "Plugin version: " + plugin.version.getStringVersion() + "\n\n"
            ;
    }
    return t;
}

bool ConfigParser::hasPlugin(const std::string& pluginName) const {
    return std::find_if(
        pluginsInfo.begin(),
        pluginsInfo.end(),
        [pluginName](PluginModel pluginModel){return pluginModel.name == pluginName;}
    ) != pluginsInfo.end();
}

}
}
