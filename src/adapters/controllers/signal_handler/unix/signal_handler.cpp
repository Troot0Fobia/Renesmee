#include <signal.h>
#include <csignal>
#include "signal_handler.hpp"

namespace adapters::controllers::signal_handler {

void internalHandler(int signal) {
    switch (signal) {
    case SIGINT:
        if (handler_)
            handler_();
        break;
    }
}

bool setHandler(std::function<bool()> handler) {
    handler_ = handler;

    struct sigaction sa{};
    sa.sa_handler = internalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (!sigaction(SIGINT, &sa, nullptr))
        return true;

    return false;
}

} // namespace adapters::controllers::signal_handler
