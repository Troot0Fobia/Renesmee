#pragma once

#include <stop_token>
#ifndef PLUGIN_H
#define PLUGIN_H

#define MAX_THREAD_COUNT 999

#include <vector>
#include "base.hpp"
#include <memory>
#include <string>
#include "input_model.hpp"

#include "plugin_info_model.hpp"
#include "plugin_api.hpp"

class Plugin {
    PluginInfoModel pluginInfo;
    std::unique_ptr<helpers::loader::Base> loader;
    PluginAPI* api = nullptr;
    std::stop_source stop_source;

    std::vector<std::string> logins;
    std::vector<std::string> passwords;
    std::vector<models::Addr> inputs;
    std::vector<models::Proxy> proxies;
    std::vector<models::Result> results;
    std::string outputPath;
    unsigned short threads;

    const std::optional<std::pair<std::string, unsigned short>> parseAddr(const std::string& addr);
    void readData(std::vector<std::string> &v, const std::string &filePath);
    void readData(std::vector<models::Addr>& v, const std::string& filePath);
    void readData(std::vector<models::Proxy>& v, const std::string& filePath);

public:
    explicit Plugin(const std::string& outputPath,
                    const std::string& inputPath,
                    const std::string& loginPath,
                    const std::string& passwordPath,
                    const std::string& proxyPath,
                    const unsigned short threads,
                    PluginInfoModel pluginPath);

    Plugin(const Plugin&) = delete;
    Plugin& operator=(const Plugin&) = delete;
    Plugin(Plugin&&) = default;
    Plugin& operator=(Plugin&&) = default;
    
    const std::string getVersion() const noexcept;
    void brute(const models::Addr& addr, const models::Proxy &proxy, std::stop_token st);
    void brute_wrapper(std::vector<models::Addr>::iterator begin_addr, std::vector<models::Addr>::iterator end_addr, models::Proxy proxy);
    void work() const;
    void printResult(const models::Result& output);

    const std::stop_source& getStopSource() const noexcept;
};

#endif // PLUGIN_H
