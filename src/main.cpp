#include <cstdlib>
#include <format>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <vector>

#include "cxxopts.hpp"
#include "toml.hpp"
#include "toml11/exception.hpp"
#include "toml11/find.hpp"
#include "toml11/parser.hpp"
#include "module_model.hpp"
#include "toml11/types.hpp"
#include "toml11/value.hpp"


#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    #define DYNAMIC_LIB_EXTENSION ".dll"
#elif defined(linux) || defined(__linux) || defined(__linux__)
    #define DYNAMIC_LIB_EXTENSION ".so"
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
    #define DYNAMIC_LIB_EXTENSION ".dylib"
#else
    #error Unknown platform
#endif

void printHelp(const cxxopts::Options& options, const std::string& additional_message = "", int exit_code = EXIT_FAILURE);
bool parseVersion(const std::string& versionString, Version& version);

int main(int argc, char* argv[]) {
    cxxopts::Options options(argv[0], "Reborn Network Scanner");
    options
        .set_width(120)
        .positional_help("<input_file.txt>")
        .add_options()
        ("h,help", "Print help message")
        ("module", "Choose module to work <hikvision>", cxxopts::value<std::string>())
        ("input", "Input file with targets", cxxopts::value<std::string>())
        ("o,output", "Output directory to save results", cxxopts::value<std::string>()->default_value("output"))
        ("proxy", "File with proxies", cxxopts::value<std::string>())
        ("l,login", "File with usernames", cxxopts::value<std::string>())
        ("password", "File with passwords", cxxopts::value<std::string>())
        ("t,threads", "Specify desired count of threads", cxxopts::value<int>()->default_value("100"))
        ;

    options.parse_positional({"input"});

    auto result = options.parse(argc, argv);

    if (result.contains("help"))
        printHelp(options, "", EXIT_SUCCESS);

    const std::map<std::string, std::string> RequiredArgs = {
        {"module", "You need to specify module for work"},
        {"input", "You need to specify input targets for work"},
        {"proxy", "You need to specify proxy for work"},
        {"login", "You need to specify usernames file for work"},
        {"password", "You need to specify passwords file for work"},
    };
    const std::string configFile = "./resources/data/modules.toml";

    for (const auto& elem : RequiredArgs)
        if (!result.contains(elem.first))
            printHelp(options, elem.second);

    toml::basic_value<toml::type_config> data;
    try {
        data = toml::parse(configFile);
    } catch (const toml::exception& err) {
        printHelp(options, std::format("Error with parsing file {}:\n{}", configFile, err.what()));
    }
    if (data.is_empty())
        printHelp(options, "You need to specify module description in ./resources/data/modules.toml");

    const auto& modules = toml::find<std::vector<toml::value>>(data, "modules");

    std::vector<ModuleModel> Modules;
    for (const auto& mod : modules) {
        Version version;
        std::string module_name = toml::find<std::string>(mod, "name");
        
        if (!parseVersion(toml::find<std::string>(mod, "version"), version))
            printHelp(options, std::format("Invalid version for module {}", module_name));
        
        Modules.push_back({
            module_name,
            toml::find<std::string>(mod, "path") + DYNAMIC_LIB_EXTENSION,
            version,
        });
    }

    for (const auto& module : Modules) {
        std::cout << "Module name: " << module.name << '\n';
        std::cout << "Module path: " << module.path << '\n';
        std::cout << "Module version: " << module.version.getStringVersion() << "\n\n";
    }

    std::cout << "Params:\n";
    for (const auto& param: result.arguments()) {
        std::cout << param.key() << ':' << param.value() << '\n';
    }

    std::cout << "\nDefaults:\n";
    for (const auto& param: result.defaults()) {
        std::cout << param.key() << ':' << param.value() << '\n';
    }

    return EXIT_SUCCESS;
}

void printHelp(const cxxopts::Options& options, const std::string& additional_message, int exit_code) {
    if (!additional_message.empty())
        std::cerr << "[ERROR] " << additional_message << "\n\n";

    std::cout << options.help() << std::endl;

    std::exit(exit_code);
}

bool parseVersion(const std::string& versionString, Version& version) {
    std::smatch match;
    if (std::regex_match(versionString, match, std::regex(R"(^(\d+)\.(\d+)\.(\d+)$)"))) {
        version.major = std::stoi(match[1].str());
        version.minor = std::stoi(match[2].str());
        version.patch = std::stoi(match[3].str());
        return true;
    }
    return false;
}
