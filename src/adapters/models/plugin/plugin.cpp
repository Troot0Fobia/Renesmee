#include "plugin.hpp"
#include "address.hpp"
#include "api_DTOs.hpp"
#include "console_args.hpp"
#include "filesystem_utils.hpp"
#include "lib_loader.hpp"
#include "plugin_api.hpp"
#include "renderer.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <exception>
#include <format>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

namespace adapters::plugin {

Plugin::Plugin(std::atomic<bool>* isStopAtomic,
               const domain::dtos::ConsoleArgs& consoleArgs,
               PluginInfo pluginInfo)
    : isStopAtomic(std::move(isStopAtomic)),
      outputPath(consoleArgs.outputPath),
      threads(consoleArgs.threadsCount),
      pluginInfo(std::move(pluginInfo)),
      logger(consoleArgs.outputPath / "output.log", consoleArgs.verboseCount) {
    readData(logins, consoleArgs.loginsPath);
    readData(passwords, consoleArgs.passwordsPath);
    readData(inputs, consoleArgs.inputPath);
    readData(proxies, consoleArgs.proxiesPath);

    std::filesystem::path output_file_path = outputPath / "results.txt";
    std::filesystem::path processed_file_path = outputPath / "processed.txt";

    output_file = std::ofstream(output_file_path,
                                std::ios_base::out | std::ios_base::app);

    if (!output_file.is_open()) {
        throw std::runtime_error(std::format(
            "Failed open output file: {}",
            output_file_path.string()));
    }

    processed_file = std::ofstream(processed_file_path,
                                   std::ios_base::out | std::ios_base::app);
    if (!processed_file.is_open()) {
        throw std::runtime_error(std::format(
            "Failed open processed file: {}",
            processed_file_path.string()));
    }

    loader = lib_loader::createLoader(
        utils::resolvePath(this->pluginInfo.path));
    auto get_plugin_api =
        loader->getFunction<PluginAPI* (*)()>("get_plugin_api");
    api = get_plugin_api();

    if (!api) {
        throw std::runtime_error(std::format(
            "Plugin {} returned null api",
            this->pluginInfo.name));
    }

    if (!api->initPlugin) {
        throw std::runtime_error(
            "Plugin's 'initPlugin' function does not specified");
    }

    if (!api->createSession) {
        throw std::runtime_error(
            "Plugin's 'createSession' function does not specified");
    }

    if (!api->changeSessionState) {
        throw std::runtime_error(
            "Plugin's 'changeSessionState' function does not specified");
    }

    if (!api->getVersion) {
        throw std::runtime_error(
            "Plugin's 'getVersion' function does not specified");
    }

    if (!api->validateAddr) {
        throw std::runtime_error(
            "Plugin's 'validateAddr' function does not specified");
    }

    if (!api->checkCreds) {
        throw std::runtime_error(
            "Plugin's 'checkCreds' function does not specified");
    }

    if (!api->shutdownPlugin) {
        throw std::runtime_error(
            "Plugin's 'shutdownPlugin' function does not specified");
    }

    if (api->initPlugin() == -1) {
        throw std::runtime_error(
            "Failed initialize plugin");
    }

    renderer = adapters::controllers::getRenderer();
    logger.info("Plugin initialized");
}

std::optional<std::pair<std::string, unsigned short>>
Plugin::parseAddr(const std::string& addr) {
    std::string ip = addr;
    unsigned short port = 0;

    size_t pos = ip.find(':');
    if (pos != std::string::npos && pos != ip.size() - 1) {
        int p = std::stoi(ip.substr(pos + 1));

        if (p > 0 && p < 65535) port = p;

        ip = ip.substr(0, pos);
    }

    std::string_view ipCopy{ip};
    int octetCount = 0;

    for (; (pos = ipCopy.find('.')) != std::string::npos &&
            pos != ipCopy.size() - 1; octetCount++) {
        int octet = std::stoi(ipCopy.substr(0, pos).data());
        if (octet < 0 || octet > 255)
            return std::nullopt;
        ipCopy = ipCopy.substr(pos + 1);
    }

    if (!ipCopy.empty()) {
        int octet = std::stoi(ipCopy.data());
        if (octet < 0 || octet > 255)
            return std::nullopt;

        octetCount++;
    }

    if (octetCount != 4)
        return std::nullopt;

    return std::make_pair(ip, port);
}

void Plugin::readData(std::vector<std::string> &v,
                      const std::filesystem::path &filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error(
            std::format("Failed open file: {}", filePath.string()));
    }

    std::string s;
    while (std::getline(file, s)) {
        if (s.empty()) continue;

        v.emplace_back(s);
    }
}

void Plugin::readData(std::queue<BruteEntity>& q,
                      const std::filesystem::path& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error(
            std::format("Failed open file: {}", filePath.string()));
    }

    std::string s;
    while (std::getline(file, s)) {
        if (s.empty()) continue;

        auto res = parseAddr(s);
        if (!res) {
            logger.warning(std::format("Invalid input address format: {}", s));
            continue;
        }

        q.emplace(Addr{res->first, res->second}, 0, 0, INTERFACE_TYPE_UNKNOWN);
    }
}

void Plugin::readData(std::vector<Proxy>& v,
                      const std::filesystem::path& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed open file: " + filePath.string());
    }

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
        if (!res) {
            logger.warning(std::format("Invalid proxy format: {}", s));
            continue;
        }

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
    const size_t inputs_amount = inputs.size();
    const size_t num_threads = std::min<size_t>({
                                    inputs_amount,
                                    proxies.size(),
                                    static_cast<size_t>(threads),
                                    MAX_THREAD_COUNT});
    workers.reserve(num_threads);
    logger.info(std::format("Threads amount: {}", num_threads));

    renderer_thread = std::jthread(
        [this, num_threads, inputs_amount]() -> void {
            renderer->emplaceData(std::format(
                              "You are using {} plugin. Version: {}\n"
                              "Loaded data:\n{}"
                              "Threads: {}\n",
                              pluginInfo.name,
                              std::string(api->getVersion()),
                                          getConfigs(),
                                          num_threads));
        auto start = std::chrono::system_clock::now();

        try {
            while (true) {
                renderer->checkSize();
                std::cout << renderer->resetPosition(true);
                unsigned short processed = this->processed,
                               valids = this->valids,
                               invalids = this->invalids;

                renderer->progressBar(static_cast<float>(processed) /
                                      static_cast<float>(inputs_amount));
                renderer->emplaceData(std::format(
                    "Goods: {}, Invalids: {}, Processed: {}",
                    valids, invalids, processed));
                renderer->emplaceData(
                    formatDuration(std::chrono::system_clock::now() - start));

                // This code does not work on Windows.
                // I don't know how to solve it.
                // It loud on fmt string and cannot evaluate it with consteval.
                // renderer->emplaceData(std::format(
                //     "[{:%T}]",
                //     std::chrono::duration_cast<std::chrono::seconds>(
                //         std::chrono::system_clock::now() - start)));

                std::cout << renderer->Print() << std::flush;
                renderer->eraseLines(3);

                if (ready_threads.load() >= num_threads) {
                    break;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        } catch (std::exception& ex) {
            logger.error(std::format("Error occured while progress bar: {}\n",
                                     ex.what()));
        }
    });

    for (size_t i = 0; i < num_threads; i++) {
        workers.emplace_back([this, i, num_threads]() -> void {
            try {
                const __Proxy __proxy =
                    this->proxies.at(i % num_threads).to_c_struct();
                const void *const session = api->createSession(&__proxy);
                if (!session) {
                    this->logger.error("Failed init session");
                    return;
                }

                while (true) {
                    BruteEntity brute_entity;
                    {
                        std::unique_lock<std::mutex> l(this->data_mutex);
                        if (this->inputs.empty() || this->isStopAtomic->load())
                            break;

                        brute_entity = std::move(this->inputs.front());
                        this->inputs.pop();
                    }

                    std::string_view ip{brute_entity.address.ip};
                    unsigned short port{brute_entity.address.port};

                    const __Addr __addr{ip.data(), port};

                    // temp solution without SYN port scan // TODO somewhen
                    if (port == 0) {
                        printProcessed(&__addr, "invalid_port");
                        this->invalids++;
                        this->processed++;
                        logger.info(std::format(
                            "Ip {} has not port specified. Skip...", ip));
                        continue;
                    }

                    if (brute_entity.interface_type == INTERFACE_TYPE_UNKNOWN) {
                        int validate_res = api->validateAddr(session,
                                                             &__addr,
                                                             this,
                                                             logHandler);
                        if (validate_res == 0) {
                            printProcessed(&__addr, "invalid_addr");
                            this->invalids++;
                            this->processed++;
                            continue;
                        } else if (validate_res == -2) {
                            std::unique_lock<std::mutex> l(this->data_mutex);
                            this->inputs.emplace(std::move(brute_entity));
                            continue;
                        }

                        brute_entity.interface_type = validate_res;
                    } else {
                        api->changeSessionState(session,
                                                "interface_type",
                                                brute_entity.interface_type);
                    }

                    [&]() -> void {
                        for (size_t login_pos = brute_entity.login_pos;
                             login_pos < this->logins.size(); login_pos++) {
                            brute_entity.login_pos = login_pos;
                            const std::string_view login{
                                this->logins[login_pos]};
                            for (size_t password_pos =
                                     brute_entity.password_pos;
                                 password_pos < this->passwords.size();
                                 password_pos++) {
                                brute_entity.password_pos = password_pos;
                                const std::string_view password{
                                    this->passwords[password_pos]};
                                if (isStopAtomic->load()) {
                                    return;
                                }

                                logger.verbose(std::format(
                                    "Trying combination {}:{} for address {}:{}",
                                    login, password, ip, port));

                                int response = api->checkCreds(session,
                                                               __Creds{login.data(),
                                                                       password.data()},
                                                               this,
                                                               logHandler,
                                                               printProcessedHandler);

                                if (response == 0) {
                                    continue;
                                } else if (response == 1) {
                                    this->valids++;
                                    logger.info(std::format(
                                        "Found valid combination {}:{} "
                                        "for address {}:{}",
                                        login, password, ip, port));
                                    printResult({
                                        brute_entity.address.ip,
                                        port,
                                        std::string(login),
                                        std::string(password)});
                                    return;
                                } else if (response == -1) {
                                    this->invalids++;
                                    return;
                                } else if (response == -2) {
                                    std::unique_lock<std::mutex> l(
                                        this->data_mutex);
                                    this->inputs.emplace(
                                        std::move(brute_entity));
                                    if (this->processed) {
                                        this->processed--;
                                    }
                                    return;
                                } else if (response == -3) {
                                    break;
                                }
                            }
                            brute_entity.password_pos = 0;
                        }
                        this->invalids++;
                        printProcessed(&__addr, "no_pass");
                    }();
                    this->processed++;
                }

                api->closeSession(session);
            } catch (const std::exception& ex) {
                logger.error(std::format("Error occured in thread: {}",
                                         ex.what()));
            }
            ready_threads++;
        });
    }
}

void Plugin::logHandler(void* ctx, PluginLogLevel level, const char* msg) {
    auto self = static_cast<Plugin*>(ctx);
    if (level == PluginLogLevel::DEBUG) {
        self->logger.debug(msg);
    } else if (level == PluginLogLevel::VERBOSE) {
        self->logger.verbose(msg);
    } else if (level == PluginLogLevel::ERROR) {
        self->logger.error(msg);
    }
}

void Plugin::printProcessedHandler(void* ctx,
                                   const __Addr* addr,
                                   const char* status) {
    auto self = static_cast<Plugin*>(ctx);
    self->printProcessed(addr, status);
}

void Plugin::printProcessed(const __Addr* const addr, const char* status) {
    std::lock_guard<std::mutex> l(processed_mutex);
    processed_file << addr->ip << ':'
                   << addr->port << " - "
                   << status << '\n' << std::flush;
}

void Plugin::printResult(const Result& output) {
    std::lock_guard<std::mutex> l(output_mutex);
    output_file << output.addr.ip << ':'
                << output.addr.port << ' '
                << output.creds.login << ':'
                << output.creds.password << '\n' << std::flush;
}

std::string Plugin::getConfigs() const noexcept {
    return std::format("Inputs: {}\n"
                       "Logins: {}\n"
                       "Passwords: {}\n"
                       "Proxies: {}\n",
                       inputs.size(), logins.size(),
                       passwords.size(), proxies.size());
}

template<typename Rep, typename Period>
std::string Plugin::formatDuration(
    const std::chrono::duration<Rep, Period> duration_time) {
    auto s =
        std::chrono::duration_cast<std::chrono::seconds>(duration_time).count();
    int h = s / 3600;
    int m = (s % 3600) / 60;
    int sec = s % 60;
    return std::format("[{:02}:{:02}:{:02}]", h, m, sec);
}

Plugin::~Plugin() {
    if (api && api->shutdownPlugin)
        api->shutdownPlugin();
}

}  // namespace adapters::plugin

