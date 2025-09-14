#include "console.hpp"
#include "cxxopts.hpp"
#include <cstdlib>
#include <iostream>
#include <string>

int parse_arguments(int argc, char *argv[], ConsoleArgs *args) {
    cxxopts::Options options("Renesmee", "Reborn Network Scanner");
    options.add_options()
        ("h,help", "Print help message")
        ("i,input", "Input file with ips", cxxopts::value<std::string>())
        ("o,output", "Output file where save results", cxxopts::value<std::string>()->default_value("output.txt"))
        ("proxy", "Provide file with proxies", cxxopts::value<std::string>())
        ("l,login", "File with usernames", cxxopts::value<std::string>())
        ("password", "File with passwords", cxxopts::value<std::string>())
        ("t,threads", "Specify count of threads to use", cxxopts::value<int>()->default_value("100"))
        ;

    auto result = options.parse(argc, argv);
    
    if (result.contains("help")) {
        std::cout << options.help() << std::endl;
        exit(EXIT_SUCCESS);
    }

    if (!result.contains("input") || !result.contains("proxy") ||
        !result.contains("login") || !result.contains("password")) {
        std::cout << "Required input file or proxy options does not provided. See usage example.\n" <<
                     "Usage: ./Renesmee -i input.txt --proxy proxy.txt -l logins.txt --password passwords.txt" << 
                     "[-o output.txt] [-t 100]" << std::endl;
        std::cout << options.help() << std::endl;
        return 1;
    }

    args->inputFile = result["input"].as<std::string>();
    args->outputFile = result["output"].as<std::string>();
    args->proxyFile = result["proxy"].as<std::string>();
    args->usernameFile = result["login"].as<std::string>();
    args->passwordFile = result["password"].as<std::string>();
    args->threads = result["threads"].as<int>();

    return 0;
}
