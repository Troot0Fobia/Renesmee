#pragma once

#include "plugin_model.hpp"
#include "toml11/find.hpp"
#include "toml11/types.hpp"
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

bool parseVersion(const std::string& versionString, Version& version);

namespace toml {
template<>
struct from<PluginModel> {
    template<typename TC>
    static PluginModel from_toml(const toml::basic_value<TC>& v) {
        std::string plugin_name = toml::find<std::string>(v, "name");
        Version ver{};
        
        if (!parseVersion(toml::find<std::string>(v, "version"), ver))
            throw std::runtime_error(
                std::format("Error parse version for {} plugin", plugin_name)
            );

        return PluginModel{
            plugin_name,
            toml::find<std::string>(v, "description"),
            toml::find<std::string>(v, "path") + DYNAMIC_LIB_EXTENSION,
            ver,
        };
    }
};
}

inline bool isUShort(int v) {
    return std::numeric_limits<unsigned short>::max() >= v && std::numeric_limits<unsigned short>::min() <= v;
}
