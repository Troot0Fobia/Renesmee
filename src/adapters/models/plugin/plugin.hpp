#pragma once

#include "console_args.hpp"
#include "address.hpp"
#include "proxy.hpp"
#include "lib_loader.hpp"
#include "plugin_info.hpp"
#include "plugin_api.hpp"
#include "result.hpp"

#include <filesystem>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#define MAX_THREAD_COUNT 999

namespace adapters::plugin {

using namespace domain::value_objects;

class Plugin {
    PluginInfo pluginInfo;
    std::unique_ptr<lib_loader::Base> loader;
    PluginAPI* api = nullptr;
    std::atomic<bool> *isStopAtomic;
    std::mutex mutex;

    std::vector<std::string> logins;
    std::vector<std::string> passwords;
    std::vector<Addr> inputs;
    std::vector<Proxy> proxies;
    std::vector<Result> results;
    std::filesystem::path outputPath;
    unsigned short threads;

    std::optional<std::pair<std::string, unsigned short>> parseAddr(const std::string& addr);
    void readData(std::vector<std::string> &v, const std::string &filePath);
    void readData(std::vector<Addr>& v, const std::string& filePath);
    void readData(std::vector<Proxy>& v, const std::string& filePath);
    void createOutput(const std::string& dirPath);

public:
    explicit Plugin(
        std::atomic<bool> *request,
        const domain::dtos::ConsoleArgs& consoleArgs,
        PluginInfo pluginPath
    );

    Plugin(const Plugin&) = delete;
    Plugin& operator=(const Plugin&) = delete;
    // Plugin(Plugin&&) = default;
    // Plugin& operator=(Plugin&&) = default;
    
    std::string getVersion() const noexcept;
    void brute(const Addr& addr, const Proxy &proxy);
    void brute_wrapper(std::vector<Addr>::iterator begin_addr, std::vector<Addr>::iterator end_addr, const Proxy& proxy);
    void work();
    void printResult(const Result& output);

    std::string getConfigs() const noexcept;
};

} // namespace adapters::plugin
