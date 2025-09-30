#include "plugin.hpp"
#include "address.hpp"
#include "api_DTOs.hpp"
#include "lib_loader.hpp"
#include "console_args.hpp"
#include "filesystem_utils.hpp"
#include "renderer.hpp"
#include <chrono>
#include <cstddef>
#include <exception>
#include <filesystem>
#include <format>
#include <iostream>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>
#include <fstream>

namespace adapters::plugin {

Plugin::Plugin(
    std::atomic<bool>* isStopAtomic,
    const domain::dtos::ConsoleArgs& consoleArgs,
    PluginInfo pluginInfo
) : isStopAtomic(std::move(isStopAtomic)), threads(consoleArgs.threadsCount),  pluginInfo(std::move(pluginInfo))
{
    readData(logins, consoleArgs.loginsPath);
    readData(passwords, consoleArgs.passwordsPath);
    readData(inputs, consoleArgs.inputPath);
    readData(proxies, consoleArgs.proxiesPath);

    outputPath = utils::createFolder(outputPath);

    output_file = std::ofstream(outputPath / "output.txt", std::ios_base::out | std::ios_base::app);

    if (!output_file.is_open()) {
        std::runtime_error(std::format("Failed open output file: {}", std::string(outputPath / "output.txt")));
    }

    loader = lib_loader::createLoader(utils::resolvePath(this->pluginInfo.path));
    auto get_plugin_api = loader->getFunction<PluginAPI* (*)()>("get_plugin_api");
    api = get_plugin_api();

    if (!api)
        throw std::runtime_error(std::format("Plugin {} returned null api", this->pluginInfo.name));

    if (!api->getVersion)
        throw std::runtime_error("Plugin's 'getVersion' function does not specified");

    if (!api->sendRequest)
        throw std::runtime_error("Plugin's 'sendRequest' function does not specified");
    
    if (!api->validateAddr)
        throw std::runtime_error("Plugin's 'validateAddr' function does not specified");
    
    renderer = adapters::controllers::getRenderer();
}

std::optional<std::pair<std::string, unsigned short>>
Plugin::parseAddr(const std::string& addr) {
    std::string ip = addr;
    unsigned short port = 0;

    size_t pos = ip.find(':');
    if (pos != std::string::npos && pos != ip.size() - 1) {
        int p = std::stoi(ip.substr(pos + 1));
        
        if (p > 0 && p < 65535)
            port = p;
            // port = static_cast<unsigned short>(p);
        
        ip = ip.substr(0, pos);
    }

    std::string ipCopy = ip;
    int octetCount = 0;

    for (; (pos = ipCopy.find('.')) != std::string::npos && pos != ipCopy.size() - 1; octetCount++) {
        int octet = std::stoi(ipCopy.substr(0, pos));
        if (octet < 0 || octet > 255)
            return std::nullopt;
        ipCopy = ipCopy.substr(pos + 1);
    }

    if (!ipCopy.empty()) {
        int octet = std::stoi(ipCopy);
        if (octet < 0 || octet > 255)
            return std::nullopt;

        octetCount++;
    }

    if (octetCount != 4)
        return std::nullopt;

    return std::make_pair(ip, port);
}

void Plugin::readData(std::vector<std::string> &v, const std::string &filePath) {
    std::ifstream file(filePath);
    if (!file.is_open())
        throw std::runtime_error(std::format("Failed open file: {}", filePath));

    std::string s;
    while (std::getline(file, s)) {
        if (s.empty()) continue;

        v.emplace_back(s);
    }
}

void Plugin::readData(std::queue<Addr>& q, const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open())
        throw std::runtime_error(std::format("Failed open file: {}", filePath));

    std::string s;
    while (std::getline(file, s)) {
        if (s.empty()) continue;

        auto res = parseAddr(s);
        if (!res) continue;

        q.emplace(res->first, res->second);
    }
}

void Plugin::readData(std::vector<Proxy>& v, const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open())
        throw std::runtime_error("Error open input file: " + filePath);

    std::string s;
    while (std::getline(file, s)) {
        if (s.empty()) continue;

        std::string username{}, password{};
        int pos = s.find('@');
        if (pos != std::string::npos) {
            int newPos = s.find(':');
            if (newPos < pos) {        
                username = s.substr(0, newPos);
                password = s.substr(newPos + 1, pos - newPos - 1);
                s = s.substr(pos + 1);
            }
        }

        auto res = parseAddr(s);
        if (!res) continue;

        v.emplace_back(Proxy{
            res->first,
            res->second,
            username,
            password,
            "http"
        });
    }
}

void Plugin::work() {
    const size_t length = inputs.size();
    const size_t num_threads = std::min({
                                    length, 
                                    proxies.size(),
                                    static_cast<size_t>(threads),
                                    MAX_THREAD_COUNT});
    workers.reserve(num_threads);

    std::jthread([this, num_threads, length]{
        renderer->emplaceData(std::format(
            "You are using {} plugin. Version: {}\n"
            "Loaded data:\n{}"
            "Threads: {}\n",
            pluginInfo.name, std::string(api->getVersion()), getConfigs(), num_threads
        ));
        auto start = std::chrono::system_clock::now();
        try {
            while (!this->isStopAtomic->load()) {
                renderer->checkSize();
                std::cout << renderer->resetPosition(true);
                unsigned short processed = this->processed,
                               valids = this->valids,
                               invalids = this->invalids;

                renderer->progressBar(static_cast<float>(processed) / static_cast<float>(length));
                renderer->emplaceData(std::format("Goods: {}, Invalids: {}, Processed: {}", valids, invalids, processed));
                renderer->emplaceData(std::format("[{:%T}]", std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start)));
                std::cout << renderer->Print() << std::flush;
                renderer->eraseLines(3);

                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        } catch (std::exception& ex) {
            std::cerr << "Error occured while progress bar: " << ex.what() << "\n";
        }
    }).detach();

    for (size_t i = 0; i < num_threads; i++) {
        workers.emplace_back([this, i, num_threads]{
            const __Proxy __proxy = this->proxies.at(i % num_threads).to_c_struct();

            while (true) {
                Addr address;
                {
                    std::unique_lock<std::mutex> lock(this->data_mutex);
                    if (this->inputs.empty() || this->isStopAtomic->load())
                        return;
                
                    address = std::move(this->inputs.front());
                    this->inputs.pop();
                }

                std::string_view ip{address.ip};
                unsigned short port = address.port;

                if (port == 0) continue; // temp solution without SYN port scan // TODO somewhen

                const __Addr __addr = {ip.data(), port};

                if (!api->validateAddr(&__addr)) continue;

                [&]{
                    for (const std::string_view login : this->logins) {
                        for (const std::string_view password : this->passwords) {
                            if (isStopAtomic->load()) return;

                            if (api->sendRequest(&__addr, &__proxy, __Creds{login.data(), password.data()})) {
                                this->valids++;
                                Result output{address.ip, port, std::string(login), std::string(password)};
                                std::lock_guard<std::mutex> l(file_mutex);
                                results.emplace_back(output);
                                printResult(output);
                                return;
                            }
                        }
                    }
                    this->invalids++;
                }();
                this->processed++;
            }
        });
    }
}

void Plugin::printResult(const Result& output) {
    output_file << output.addr.ip << ':' << output.addr.port << ' ' << output.creds.login << ':' << output.creds.password << std::endl;
}

std::string Plugin::getConfigs() const noexcept {
    return std::format("Inputs: {}\n"
                       "Logins: {}\n"
                       "Passwords: {}\n"
                       "Proxies: {}\n",
                       inputs.size(), logins.size(), passwords.size(), proxies.size()
                    );
}

} // namespace adapters::plugin
