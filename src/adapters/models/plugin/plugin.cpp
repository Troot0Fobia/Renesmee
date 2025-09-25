#include "plugin.hpp"
#include "address.hpp"
#include "api_DTOs.hpp"
#include "lib_loader.hpp"
#include "console_args.hpp"
#include "filesystem_utils.hpp"
#include <filesystem>
#include <format>
#include <iostream>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
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
    createOutput(consoleArgs.outputPath);

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
}

std::string Plugin::getVersion() const noexcept {
    return std::string(api->getVersion());
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

void Plugin::createOutput(const std::string& dirPathStr) {
    if (dirPathStr.empty())
        throw std::runtime_error("Output path cannot be empty");

    std::error_code err_code;
    outputPath = std::filesystem::weakly_canonical(
        std::filesystem::path(dirPathStr),
        err_code
    );

    if (err_code)
        throw std::filesystem::filesystem_error(
            std::format("Error occured with path {}: {}", dirPathStr, err_code.message()),
            err_code
        );

    if (std::filesystem::exists(outputPath))
        return;

    if (!std::filesystem::create_directories(outputPath, err_code))
        throw std::filesystem::filesystem_error(
            std::format(
                "Failed create directories with path {}: {}",
                outputPath.string(),
                err_code.message()
            ),
            err_code
        );

    std::cout << "Created folder: " << outputPath << std::endl;
}

void Plugin::work() {
    size_t const num_threads = std::min({
                                    inputs.size(),
                                    proxies.size(),
                                    (size_t)threads,
                                    (size_t)MAX_THREAD_COUNT});
    workers.reserve(num_threads);

    for (size_t i = 0; i < num_threads; i++) {
        workers.emplace_back([this, i, num_threads]{
            const __Proxy __proxy = this->proxies.at(i % num_threads).to_c_struct();

            while (true) {
                Addr address;
                {
                    std::unique_lock<std::mutex> lock(this->mutex);
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
                            std::cout << "[i] Trying combination for address [" << ip << ":" << port << "] " << login << "/" << password << "\n";

                            if (api->sendRequest(&__addr, &__proxy, __Creds{login.data(), password.data()})) {
                                std::cout << "[+] Found creds for address [" << ip << ":" << port << "] " << login << "/" << password << "\n";
                                Result output{address.ip, port, std::string(login), std::string(password)};
                                results.emplace_back(output);
                                printResult(output);
                                return;
                            }
                        }
                    }
                }();
            }
        });
    }
}

void Plugin::printResult(const Result& output) {
    std::lock_guard<std::mutex> l(mutex);
    std::ofstream file(outputPath / "output.txt", std::ios_base::out | std::ios_base::app);

    if (!file.is_open()) {
        // std::cerr << "Unable open file " << outputFile << std::endl;
        return;
    }

    file << output.addr.ip << ':' << output.addr.port << ' ' << output.creds.login << ':' << output.creds.password << '\n';
}

std::string Plugin::getConfigs() const noexcept {
    return std::format("Loaded data:\n"
                       "Inputs: {}\n"
                       "Logins: {}\n"
                       "Passwords: {}\n"
                       "Proxies: {}\n",
                       inputs.size(), logins.size(), passwords.size(), proxies.size()
                    );
}

} // namespace adapters::plugin
