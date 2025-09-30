#include <atomic>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <utility>

#include "config.hpp"
#include "console.hpp"
#include "plugin.hpp"
#include "plugin_info.hpp"
#include "signal_handler.hpp"

int main(int argc, char* argv[]) {
    std::atomic<bool> stop{false};
    std::string configFile = "./configs/plugins.toml";

    adapters::controllers::signal_handler::setHandler([&stop]{
        stop.store(true);
        return true;
    });

    try {
        adapters::console::ConsoleParser console_parser(argc, argv);

        if (console_parser.hasOption("help")) {
            std::cout << console_parser.help() << std::endl;
            return EXIT_SUCCESS;
        }

        if (std::string configPath = console_parser.getOption<std::string>("config");
            !configPath.empty())
        {
            configFile = configPath;
        }

        adapters::config::ConfigParser config_parser(configFile);

        if (console_parser.hasOption("show-plugins")) {
            std::cout << config_parser.pluginInfo() << std::endl;
            return EXIT_SUCCESS;
        }

        if (auto missing = console_parser.checkRequired();
            !missing.empty())
        {
            std::cerr << "Required parameter was not specified:\n";
            
            for (const auto& [argument, message] : missing)
                std::cerr << argument << " : " << message << "\n";
            std::cerr << console_parser.help() << std::endl;
            return EXIT_FAILURE;
        }

        const std::string pluginName{console_parser.getOption<std::string>("plugin")};
        auto plugin_info_opt = config_parser.getPlugin(pluginName);
        
        if (!plugin_info_opt) {
            std::cerr << "[ ERROR ] Plugin you specified \"" << pluginName << "\" does not exist" << "\n\n"
                      << "Available plugins:\n"
                      << config_parser.pluginInfo() << std::endl;
            return EXIT_FAILURE;
        }

        adapters::plugin::Plugin plugin(
            &stop,
            console_parser.getArgs(),
            std::move(*plugin_info_opt)
        );

        plugin.work();

    } catch (const std::exception& ex) {
        std::cerr << "An error occured while program lifecycle:\n"
                  << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    stop.store(true);
	return EXIT_SUCCESS;
}
