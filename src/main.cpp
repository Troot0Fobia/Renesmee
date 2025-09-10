#include "Console/console.hpp"
#include "Renesmee/renesmee.hpp"
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include "ctrl_c.h"


int main(int argc, char *argv[]) {
    ConsoleArgs consoleArgs;

    if (parse_arguments(argc, argv, &consoleArgs))
        return EXIT_FAILURE;

    try {
        Renesmee renesmee(
            consoleArgs.inputFile,
            consoleArgs.outputFile,
            consoleArgs.proxyFile,
            consoleArgs.usernameFile,
            consoleArgs.passwordFile,
            consoleArgs.threads
        );

        auto& stop_src = renesmee.getStopSource();

        unsigned int hadler_id = CtrlCLibrary::SetCtrlCHandler([&stop_src](CtrlCLibrary::CtrlSignal event) {
            switch (event) {
                case CtrlCLibrary::kCtrlCSignal:
                    std::cout << "Caught Ctrl+C signal. Stopping threads..." << std::endl;
                    stop_src.request_stop();
                    break;
            }
            return true;
        });

        std::cout << "Count of input data: " << renesmee.loadInputData() << std::endl;
        std::cout << "Count of proxy data: " << renesmee.loadProxyData() << std::endl;
        std::cout << "Count of login data: " << renesmee.loadUsernameData() << std::endl;
        std::cout << "Count of password data: " << renesmee.loadPasswordData() << std::endl;
        
        // renesmee.printConfiguration();
        renesmee.run();

        std::cout << std::endl;

        renesmee.printResults();

        CtrlCLibrary::ResetCtrlCHandler(hadler_id);
    } catch (std::runtime_error e) {
        std::cerr << "Error while configuring: " << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
