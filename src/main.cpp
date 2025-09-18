#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

#include "base.hpp"
#include "config.hpp"
#include "console.hpp"
#include "plugin_model.hpp"


int main(int argc, char* argv[]) {
    std::string configFile = "./resources/data/plugins.toml";
    
    try {
        helpers::console::ConsoleParser console_parser(argc, argv);

        if (console_parser.hasOption("help")) {
            std::cout << console_parser.help() << std::endl;
            return EXIT_SUCCESS;
        }

        if (std::string path = console_parser.getOption<std::string>("config"); !path.empty()) {
            configFile = path;
        }

        helpers::config::ConfigParser config_parser(configFile);

        if (console_parser.hasOption("show_plugins")) {
            std::cout << config_parser.pluginInfo() << std::endl;
            return EXIT_SUCCESS;
        }

        const auto missing = console_parser.checkRequired();
        if (!missing.empty()) {
            std::cerr << "Required parameter was not specified:\n";
            for (const auto& arg : missing)
                std::cerr << arg.argument << " : " << arg.message << "\n";
            std::cerr << console_parser.help() << std::endl;
            return EXIT_FAILURE;
        }

        const std::string pluginName{console_parser.getOption<std::string>("plugin")};
        auto plugin_opt = config_parser.getPlugin(pluginName);
        
        if (!plugin_opt) {
            std::cerr << "[ ERROR ] Plugin you specified \"" << pluginName << "\" does not exist" << "\n\n"
                      << "Available plugins:\n"
                      << config_parser.pluginInfo() << std::endl;
            return EXIT_FAILURE;
        }

        const PluginModel& plugin = plugin_opt->get();
        
        auto loader = helpers::loader::createLoader(plugin.path);
        

    } catch (const std::exception& ex) {
        std::cerr << "An error occured while program lifecycle:\n"
                  << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

	return EXIT_SUCCESS;
}
