#include "console.hpp"
#include <string>
#include <vector>
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
}

bool ConsoleParser::hasOption(const std::string& opt) const {
    return result.contains(opt);
}

std::vector<MissingArgument> ConsoleParser::checkRequired() const {
    const std::map<std::string, std::string> RequiredArgs = {
        {"plugin",   "You need to specify plugin for work"},
        {"input",    "You need to specify input targets for work"},
        {"proxy",    "You need to specify proxy for work"},
        {"login",    "You need to specify usernames file for work"},
        {"password", "You need to specify passwords file for work"}
    };

    std::vector<MissingArgument> missing;
    for (const auto& [argument, message] : RequiredArgs)
        if (!result.contains(argument))
            missing.push_back({argument, message});

    return missing;
}

std::string ConsoleParser::help() const {
    return options.help();
}

} // console
} // helpers