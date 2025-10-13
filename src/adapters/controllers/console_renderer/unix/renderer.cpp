#include "renderer.hpp"
#include <csignal>
#include <atomic>
#include <memory>
#include <sys/ioctl.h>

namespace adapters::controllers {

class Renderer : public BaseRenderer {
    inline static std::atomic<bool> resized{false};

    static void signal_handler(int signal) {
        if (signal == SIGWINCH) {
            resized.store(true);
        }
    }

    void refreshSize() {
        struct winsize w;
        if (!ioctl(STDOUT_FILENO, TIOCGWINSZ, &w)) {
            width = w.ws_col;
            height = w.ws_row;
        } else {
            width = 80;
            height = 24;
        }
    }

public:
    Renderer() {
        struct sigaction sa;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;
        sa.sa_handler = signal_handler;

        sigaction(SIGWINCH, &sa, nullptr);

        refreshSize();
    }

    void checkSize() override {
        if (resized.load()) {
            resized.store(false);
            refreshSize();
        }
    }
};

std::unique_ptr<BaseRenderer> getRenderer() {
    return std::make_unique<Renderer>();
}

}  // namespace adapters::controllers

