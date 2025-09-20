#include "config.hpp"
#include <algorithm>
#include <filesystem>
#include <format>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include "plugin_info_model.hpp"
#include "toml.hpp"
#include "toml11/find.hpp"
#include "toml11/value.hpp"
#include "filesystem_resolver.hpp"

#include "toml_from.hpp" // need for override method toml::from

namespace helpers {
namespace config {

ConfigParser::ConfigParser(const std::string& configPath) {
    const std::filesystem::path cannonical_path = resolvePath(configPath);

    auto res = toml::try_parse(cannonical_path);
    
    if (res.is_ok())
        pluginsInfo = toml::find<std::vector<PluginInfoModel>>(std::move(res.unwrap()), "plugins");
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

std::optional<PluginInfoModel>
ConfigParser::getPlugin(const std::string& pluginName) {
    auto it = std::ranges::find_if(
        pluginsInfo,
        [&pluginName](const PluginInfoModel& pluginInfoModel){
            return pluginInfoModel.name == pluginName;
        }
    );

    if (it != pluginsInfo.end()) {
        PluginInfoModel pluginInfo = std::move(*it);
        pluginsInfo.erase(it);
        return pluginInfo;
    }

    return std::nullopt;
}

}
}
