#include "config.hpp"
#include <algorithm>
#include <filesystem>
#include <format>
#include <functional>
#include <optional>
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

namespace helpers {
namespace config {

ConfigParser::ConfigParser(const std::string& configPath) {
    if (configPath.empty())
        throw std::invalid_argument("Config path cannot be empty");

    std::error_code err_code;
    std::filesystem::path path(configPath);

    if (!std::filesystem::exists(path, err_code))
        throw std::filesystem::filesystem_error(
            std::format("Provided path {} does not exist", configPath),
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

std::optional<std::reference_wrapper<const PluginModel>>
ConfigParser::getPlugin(const std::string& pluginName) const {
    auto it = std::ranges::find_if(
        pluginsInfo,
        [&pluginName](const PluginModel& pluginModel){
            return pluginModel.name == pluginName;
        }
    );

    if (it != pluginsInfo.end())
        return std::cref(*it);

    return std::nullopt;
}

}
}
