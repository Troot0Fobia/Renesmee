#include "base.hpp"
#include "filesystem_resolver.hpp"
#include <Windows.h>
#include <filesystem>
#include <format>
#include <memory>
#include <stdexcept>

namespace helpers {
namespace loader {

class WinLoader : public Base {
    HMODULE handler;

protected:
    void* getFunctionVoid(const std::string& name) const override {
        FARPROC ptr = GetProcAddress(handler, name.c_str());
        if (!ptr)
            throw std::runtime_error(std::format("Failed receive method {}\n", name));

        return (void*)ptr;
    }

public:
    explicit WinLoader(const std::string& path) {
        const std::filesystem::path cannonical_path = resolvePath(path);
        handler = LoadLibrary(cannonical_path.string().c_str());

        if (!handler)
            throw std::runtime_error(std::format("Failed to open dynamic library {}\n", cannonical_path.string()));
    }

    ~WinLoader() override {
        if (handler)
            FreeLibrary(handler);
    }
};

std::unique_ptr<Base> createLoader(const std::string &path) {
    return std::make_unique<WinLoader>(path);
}

} // loader
} // helpers