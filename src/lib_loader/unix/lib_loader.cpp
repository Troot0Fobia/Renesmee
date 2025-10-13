#include "lib_loader.hpp"
#include <dlfcn.h>
#include <format>
#include <stdexcept>
#include <string>
#include <memory>

namespace lib_loader {

class UnixLoader : public Base {
    void* handler = nullptr;

protected:
    void* getFunctionVoid(const std::string& name) const override {
        void* ptr = dlsym(handler, name.c_str());
        if (!ptr)
            throw std::runtime_error(std::format("Failed receive method {}:\n", name, dlerror()));

        dlerror();
        return ptr;
    }

public:
    explicit UnixLoader(const std::filesystem::path& path) {
        handler = dlopen(path.c_str(), RTLD_NOW);

        if (!handler)
            throw std::runtime_error(std::format("Failed to open dynamic library {}:\n{}", path.string(), dlerror()));

        dlerror();
    }

    ~UnixLoader() override {
        if (handler)
            dlclose(handler);
    }
};

std::unique_ptr<Base> createLoader(const std::filesystem::path& path) {
    return std::make_unique<UnixLoader>(path);
}

}  // namespace lib_loader

