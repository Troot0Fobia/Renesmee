#include "config.hpp"
#include <filesystem>
#include <format>
#include <regex>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>
#include <vector>
#include "plugin_model.hpp"
#include "toml11/find.hpp"
#include "toml11/value.hpp"

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
        data = std::move(res.unwrap());
    else
        throw std::runtime_error(
            std::format("TOML error parse file {}",
                res.unwrap_err().front().title()
            )
        );

    pluginsInfo = toml::find<std::vector<PluginModel>>(data, "plugins");
}

std::vector<PluginModel> ConfigParser::parse() {
    std::vector<PluginModel> plugins;

    const auto plugs = toml::find<std::vector<toml::value>>(data, "plugins");

    for (const auto& plugin : plugs) {
        Version ver;
        std::string plugin_name = toml::find<std::string>(plugin, "name");
        
        if (!parseVersion(toml::find<std::string>(plugin, "version"), ver))
            throw std::runtime_error(
                std::format("Error parse version for {} plugin", plugin_name)
            );

        plugins.push_back({
            plugin_name,
            toml::find<std::string>(plugin, "description"),
            toml::find<std::string>(plugin, "path") + DYNAMIC_LIB_EXTENSION,
            ver,
        });
    }

    // plugins.push_back(toml::find<PluginModel>(data, "plugins", 0));
    return plugins;
}
// return std::vector<PluginModel>{};

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

inline bool isUShort(int v) {
    return std::numeric_limits<ushort>::max() >= v && std::numeric_limits<ushort>::min() <= v;
}


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

}
}
