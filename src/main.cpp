#include <cstdlib>
#include <iostream>

#include "config.hpp"
#include "console.hpp"


int main(int argc, char* argv[]) {
    const std::string configFile = "./resources/data/plugins.toml";
    
	helpers::console::ConsoleParser console_parser(argc, argv);

    if (console_parser.hasOption("help")) {
        console_parser.printHelp("", EXIT_SUCCESS);
    }

	helpers::config::ConfigParser config_parser(configFile);
	// std::vector<PluginModel> plugins = config_parser.parse();

    if (console_parser.hasOption("show_plugins")) {
        std::cout << config_parser.pluginInfo() << std::endl;
    }

	// std::cout << config_parser.pluginInfo(plugins) << std::endl;


	// bool show_plugins{false};


	// const auto& plugins = toml::find<std::vector<toml::value>>(data, "plugins");
	// // bool find_module{false};
	// // std::string user_module = result["module"].as<std::string>();
	// std::vector<PluginModel> Plugins;

	// for (const auto& mod : plugins) {
        // Version version;
        // std::string module_name = toml::find<std::string>(mod, "name");
        // // if (user_module.compare(module_name) == 0) {
        // // find_module = true;
        // // }

        // if (!parseVersion(toml::find<std::string>(mod, "version"), version))
            // printHelp(options, std::format("Invalid version for module {}", module_name));

        // Plugins.push_back({
            // module_name,
            // toml::find<std::string>(mod, "description"),
            // toml::find<std::string>(mod, "path") + DYNAMIC_LIB_EXTENSION,
            // version,
        // });

	// }

	// if (show_plugins) {
        // for (const auto& module : Plugins) {
            // std::cout << module.name << " - " << module.descpription << '\n';
        // }

	// return EXIT_SUCCESS;

	// }

	// const std::map<std::string, std::string> RequiredArgs = {
        // {"module", "You need to specify module for work"},
        // {"input", "You need to specify input targets for work"},
        // {"proxy", "You need to specify proxy for work"},
        // {"login", "You need to specify usernames file for work"},
        // {"password", "You need to specify passwords file for work"},
	// };

	// for (const auto& elem : RequiredArgs)
	    // if (!result.contains(elem.first))
	        // printHelp(options, elem.second);

	// // if (!find_module) {
	    // // std::cerr << "[ERROR] You specified wrong module name: " << user_module << '\n' <<
        // // "Possible modules:\n";
        // // for (const auto& module : Modules) {
        // // std::cerr << module.name << " - " << module.descpription << '\n';
        // // }
        // // return EXIT_FAILURE;
    // // }

	// for (const auto& module : Plugins) {
        // std::cout << "Module name: " << module.name << '\n';
        // std::cout << "Module path: " << module.path << '\n';
        // std::cout << "Module version: " << module.version.getStringVersion() << "\n\n";
	// }

	// std::cout << "Params:\n";
    // for (const auto& param: result.arguments()) {
        // std::cout << param.key() << ':' << param.value() << '\n';
	// }

	// std::cout << "\nDefaults:\n";
    // for (const auto& param: result.defaults()) {
        // std::cout << param.key() << ':' << param.value() << '\n';
	// }

	return EXIT_SUCCESS;
}
