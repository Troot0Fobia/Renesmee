#pragma once

#ifndef MODULE_MODEL_H
#define MODULE_MODEL_H

#include <tuple>
#include <string>
#include <format>

struct Version {
    int major;
    int minor;
    int patch;

    const std::string getStringVersion() const {
        return std::format("{}.{}.{}", major, minor, patch);
    }

    bool operator==(const Version& other) const {
        return std::tie(major, minor, patch) == std::tie(other.major, other.minor, other.patch);
    }
};

struct PluginInfoModel {
    std::string name;
    std::string description;
    std::string path;
    Version version;
};

#endif // MODULE_MODEL_H
