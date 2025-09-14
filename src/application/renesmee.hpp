#pragma once
#include <mutex>
#include <stop_token>
#include <string>
#include <vector>
#ifndef RENESMEE_H
#define RENESMEE_H

#define MAX_THREAD_COUNT 999

struct InetAddr {
    std::string ip;
    int port = 0;
};

struct Creds {
    std::string username;
    std::string password;
};

struct InetData {
    Creds creds;
    InetAddr addr;
};

typedef InetData Proxy;
typedef InetData Output;


class Renesmee {
    std::string inputFile;
    std::string outputFile;
    std::string proxyFile;
    std::string loginFile;
    std::string passwordFile;
    int threadsCount;

    std::mutex mutex;
    std::stop_source stop_source;

    std::vector<InetAddr> inputData;
    std::vector<Proxy> proxyData;
    std::vector<Output> outputData;
    std::vector<std::string> loginData;
    std::vector<std::string> passwordData;

    bool checkFileExist(const std::string&);
    void brute(const InetAddr&, const Proxy&, std::stop_token);
    void brute_wrapper(std::vector<InetAddr>::iterator begin, std::vector<InetAddr>::iterator end, Proxy proxy);
    std::pair<std::string, int> parseAddr(const std::string &s);
    void printResult(const Output& output);

public:
    Renesmee(
        std::string inputFile,
        std::string outputFile,
        std::string proxyFile,
        std::string loginFile,
        std::string passwordFile,
        int threadsCount
    );

    std::string getInputFile();
    std::string getOutputFile();
    std::string getProxyFile();
    int getThreadsCount();

    std::stop_source& getStopSource();

    int loadInputData();
    int loadProxyData();
    int loadUsernameData();
    int loadPasswordData();

    void run();

    void printResults();

    void printConfiguration();
};

#endif // RENESMEE_H
