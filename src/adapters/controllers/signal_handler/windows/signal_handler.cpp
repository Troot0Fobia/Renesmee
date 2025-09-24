#include "signal_handler.hpp"
#include <Windows.h>

namespace adapters::controllers::signal_handler {

BOOL WINAPI internalHandler(DWORD dwCtrlType) {
    switch (dwCtrlType) {
    case CTRL_C_EVENT:
        if (handler_)
            return handler_();
    }
}

bool setHandler(std::function<bool()> handler) {
    handler_ = handler;
    return SetConsoleCtrlHandler(internalHandler, TRUE) ? TRUE : FALSE;
}


} // namespace adapters::controllers::signal_handler
