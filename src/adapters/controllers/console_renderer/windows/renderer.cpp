#include <memory>
#include <renderer.hpp>
#include <stdexcept>
#include <Windows.h>

namespace adapters::controllers {

class Renderer : public BaseRenderer {
    HANDLE hOut;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD oldSize = {0, 0};

public:
    Renderer() {
        hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut == INVALID_HANDLE_VALUE) {
            throw std::runtime_error("Unable receive console handler");
        }
        DWORD mode = 0;
        if (GetConsoleMode(hOut, &mode)) {
            mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, mode);
        }

        checkSize();
    }

    void checkSize() override {
        if (GetConsoleScreenBufferInfo(hOut, &csbi)) {
            COORD newSize = {
                csbi.srWindow.Right - csbi.srWindow.Left + 1,
                csbi.srWindow.Bottom -  csbi.srWindow.Top + 1
            };

            if (newSize.X != oldSize.X || newSize.Y != oldSize.Y) {
                width = newSize.X;
                height = newSize.Y;
                oldSize = newSize;
            }
        } else {
            width = 80;
            height = 24;
        }
    }
};

std::unique_ptr<BaseRenderer> getRenderer() {
    return std::make_unique<Renderer>();
}

} // namespace adapters::controllers
