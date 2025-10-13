#pragma once

#include <memory>
#include <string>
#include <vector>
#include <format>
#include <sstream>

namespace adapters::controllers {

class BaseRenderer {
protected:
    int height;
    int width;
    std::vector<std::string> screen;

public:
    void emplaceData(const std::string& data) {
        screen.emplace_back(data);
    }

    void emplaceData(std::string&& data) {
        screen.emplace_back(std::move(data));
    }

    void fill(char character) {
        screen.emplace_back(std::string(width, character));
    }

    void progressBar(float fraction) {
        std::stringstream ss;
        ss << '[';
        std::string closer = std::format("] {:2d}%", static_cast<int>(fraction * 100));
        int available = width - closer.size() - ss.gcount() - 1;
        int equals = available * fraction;
        int under_dashes = available - equals;
        ss << std::string(equals, '=') << std::string(under_dashes, '_') << closer;
        screen.emplace_back(ss.str());
    }

    void eraseLines(int count) {
        screen.resize(screen.size() - count);
    }

    std::string resetPosition(bool clear) {
        std::stringstream ss;
        if (clear)
            ss << "\x1B[2J";

        ss << "\x1B[H";
        return ss.str();
    }

    std::string Print() {
        std::stringstream ss;
        for (const std::string& data : screen)
            ss << data << "\n";

        return ss.str();
    }

    virtual void checkSize() = 0;

    virtual ~BaseRenderer() = default;
};

std::unique_ptr<BaseRenderer> getRenderer();

}  // namespace adapters::controllers

