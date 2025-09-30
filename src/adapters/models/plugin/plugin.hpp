#pragma once

#include "console_args.hpp"
#include "address.hpp"
#include "proxy.hpp"
#include "lib_loader.hpp"
#include "plugin_info.hpp"
#include "plugin_api.hpp"
#include "result.hpp"
#include "renderer.hpp"

#include <filesystem>
#include <fstream>
#include <mutex>
#include <optional>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#define MAX_THREAD_COUNT 999UL

namespace adapters::plugin {

using namespace domain::value_objects;

class Plugin {
    PluginInfo pluginInfo;
    std::unique_ptr<lib_loader::Base> loader;
    PluginAPI* api = nullptr;
    std::atomic<bool> *isStopAtomic;
    std::atomic<unsigned int> processed{};
    std::atomic<unsigned int> invalids{};
    std::atomic<unsigned int> valids{};
    std::mutex data_mutex;
    std::mutex file_mutex;
    std::ofstream output_file;

    std::vector<std::string> logins;
    std::vector<std::string> passwords;
    std::queue<Addr> inputs;
    std::vector<Proxy> proxies;
    std::vector<Result> results;
    std::filesystem::path outputPath;
    std::unique_ptr<controllers::BaseRenderer> renderer;

    std::vector<std::jthread> workers;
    unsigned short threads;

    std::optional<std::pair<std::string, unsigned short>> parseAddr(const std::string& addr);
    void readData(std::vector<std::string> &v, const std::string &filePath);
    void readData(std::queue<Addr>& q, const std::string& filePath);
    void readData(std::vector<Proxy>& v, const std::string& filePath);

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
    
    void work();
    void printResult(const Result& output);

    std::string getConfigs() const noexcept;
};

} // namespace adapters::plugin
