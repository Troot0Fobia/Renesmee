#include "base.hpp"
#include <Windows.h>
#include <format>
#include <stdexcept>

namespace lib_loader {

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
    explicit WinLoader(const std::filesystem::path& path) {
        // const std::filesystem::path cannonical_path = resolvePath(path);
        handler = LoadLibrary(path.string().c_str());

        if (!handler)
            throw std::runtime_error(std::format("Failed to open dynamic library {}\n", path.string()));
    }

    ~WinLoader() override {
        if (handler)
            FreeLibrary(handler);
    }
};

std::unique_ptr<Base> createLoader(const std::filesystem::path& path) {
    return std::make_unique<WinLoader>(path);
}

} // namespace lib_loader
