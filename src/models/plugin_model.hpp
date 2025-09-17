#pragma once

#include <tuple>
#ifndef MODULE_MODEL_H
#define MODULE_MODEL_H

#include <string>
#include <format>

struct Version {
    int major;
    int minor;
    int patch;

    std::string getStringVersion() const {
        return std::format("{}.{}.{}", major, minor, patch);
    }

    bool operator==(const Version& other) {
        return std::tie(major, minor, patch) == std::tie(other.major, other.minor, other.patch);
    }
};

struct PluginModel {
    std::string name;
    std::string description;
    std::string path;
    Version version;
};

#endif // MODULE_MODEL_H