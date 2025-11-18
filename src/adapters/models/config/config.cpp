#include "config.hpp"
#include "filesystem_utils.hpp"
#include "plugin_info.hpp"
#include "toml.hpp"
#include "toml11/find.hpp"
#include "toml11/value.hpp"
#include "version.hpp"
#include <algorithm>
#include <format>
#include <regex>
#include <stdexcept>
#include <string>
#include <utility>

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    #define DYNAMIC_LIB_EXTENSION ".dll"
#elif defined(linux) || defined(__linux) || defined(__linux__)
    #define DYNAMIC_LIB_EXTENSION ".so"
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
    #define DYNAMIC_LIB_EXTENSION ".dylib"
#else
    #error Unknown platform
#endif

std::optional<Version> parseVersion(const std::string& versionString) {
    std::smatch match;
    int t;
    unsigned short major, minor, patch;
    if (std::regex_match(versionString, match, std::regex(R"(^(\d+)\.(\d+)\.(\d+)$)"))) {
        t = std::stoi(match[1].str());
        if (t < 0 || t > 65535)
            return std::nullopt;

        major = t;
        // major = static_cast<unsigned short>(t);

        t = std::stoi(match[2].str());
        if (t < 0 || t > 65535)
            return std::nullopt;

        minor = t;
        // minor = static_cast<unsigned short>(t);

        t = std::stoi(match[3].str());
        if (t < 0 || t > 65535)
            return std::nullopt;

        patch = t;
        // patch = static_cast<unsigned short>(t);

        return Version{major, minor, patch};
    }
    return std::nullopt;
}

namespace toml {
template<>
struct from<PluginInfo> {
    template<typename TC>
    static PluginInfo from_toml(const toml::basic_value<TC>& v) {
        const std::string plugin_name{toml::find<std::string>(v, "name")};

        auto ver_opt = parseVersion(toml::find<std::string>(v, "version"));
        if (!ver_opt)
            throw std::runtime_error(
                std::format("Error parse version for {} plugin", plugin_name));

        return PluginInfo{
            plugin_name,
            toml::find<std::string>(v, "description"),
            toml::find<std::string>(v, "path") + DYNAMIC_LIB_EXTENSION,
            std::move(*ver_opt),
        };
    }
};
} // namespace toml

namespace adapters::config {

ConfigParser::ConfigParser(const std::string& configPath) {
    const std::filesystem::path cannonical_path = utils::resolvePath(configPath);

    auto res = toml::try_parse(cannonical_path);

    if (res.is_ok()) {
        pluginsInfo = toml::find<std::vector<PluginInfo>>(std::move(res.unwrap()),
                                                          "plugins");
    } else {
        throw std::runtime_error(
            std::format(
                "TOML error parse file: {}",
                res.unwrap_err().front().title()));
    }
}

std::string ConfigParser::pluginInfo() const noexcept {
    std::stringstream ss;

    for (const auto& plugin : pluginsInfo) {
        ss << "Plugin name: " << plugin.name << "\n" <<
              "Plugin description: " << plugin.description << "\n" <<
              "Plugin path: " << plugin.path << "\n" <<
              "Plugin version: " << plugin.version.getStringVersion() << "\n\n";
    }

    return ss.str();
}

std::optional<PluginInfo>
ConfigParser::getPlugin(const std::string& pluginName) {
    auto it = std::ranges::find_if(
        pluginsInfo,
        [&pluginName](const PluginInfo& pluginInfo){
            return pluginInfo.name == pluginName;
        });

    if (it != pluginsInfo.end()) {
        PluginInfo pluginInfo = std::move(*it);
        pluginsInfo.clear();
        return pluginInfo;
    }

    return std::nullopt;
}

} // namespace adapters::config
