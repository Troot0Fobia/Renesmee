#include "console.hpp"
#include <iostream>
// #include <map>
#include "cxxopts.hpp"

namespace helpers {
namespace console {
    
ConsoleParser::ConsoleParser(int argc, char** argv)
    : options(argv[0], "Reborn Network Scanner")
{
    options
        .set_width(120)
        .positional_help("<input_file.txt>")
        .custom_help("[options...]")
        .add_options()
        ("h,help", "Print help message")
        ("show_plugins", "Print possible plugins with description", cxxopts::value<bool>())
        ("plugin", "Plugin for work", cxxopts::value<std::string>(), "<plugin_name>")
        ("input", "Input file with targets", cxxopts::value<std::string>())
        ("o,output", "Output directory to save results", cxxopts::value<std::string>()->default_value("output"), "<folder>")
        ("proxy", "File with proxies", cxxopts::value<std::string>(), "<file>")
        ("l,login", "File with usernames", cxxopts::value<std::string>(), "<file>")
        ("password", "File with passwords", cxxopts::value<std::string>(), "<file>")
        ("t,threads", "Specify desired count of threads", cxxopts::value<int>()->default_value("100"), "<n>")
        ;

    options.parse_positional({"input"});

    result = options.parse(argc, argv);

    // if (result.contains("help"))
    //     printHelp("", EXIT_SUCCESS);

    // std::map<std::string, std::string> RequiredArgs = {
    //     {"plugin", "You need to specify plugin for work"},
    //     {"input", "You need to specify input targets for work"},
    //     {"proxy", "You need to specify proxy for work"},
    //     {"login", "You need to specify usernames file for work"},
    //     {"password", "You need to specify passwords file for work"}
    // };

    // for (const auto& p : RequiredArgs) {
    //     if (!result.contains(p.first))
    //         printHelp(p.second);
    // }
}

bool ConsoleParser::hasOption(const std::string& opt) const {
    return result.contains(opt);
}

template<typename T>
T ConsoleParser::getOption(const std::string& opt) const {
    return result[opt].as<T>();
}

void ConsoleParser::printHelp(const std::string& additional_message, int exit_code) {
    if (!additional_message.empty())
        std::cerr << "[ ERROR ] " << additional_message << "\n\n";

    std::cout << options.help() << std::endl;

    std::exit(exit_code);
}
    
} // console
} // helpers