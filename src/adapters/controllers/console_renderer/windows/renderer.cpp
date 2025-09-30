#include <memory>
#include <renderer.hpp>
#include <Windows.h>

namespace adapters::controllers {

class Renderer : public BaseRenderer {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD oldSize = {0, 0};

public:
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
