#include "signal_handler.hpp"
#include <Windows.h>

namespace adapters::controllers::signal_handler {

BOOL WINAPI internalHandler(DWORD dwCtrlType) {
    switch (dwCtrlType) {
    case CTRL_C_EVENT:
        if (handler_)
            handler_();
        return TRUE;
    }
    return FALSE;
}

bool setHandler(std::function<void()> handler) {
    handler_ = handler;
    return SetConsoleCtrlHandler(internalHandler, TRUE) ? TRUE : FALSE;
}


} // namespace adapters::controllers::signal_handler
