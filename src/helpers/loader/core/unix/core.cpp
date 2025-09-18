#include "base.hpp"
#include "path_resolver.hpp"
#include <dlfcn.h>
#include <filesystem>
#include <format>
#include <memory>
#include <stdexcept>

namespace helpers {
namespace loader {

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
    explicit UnixLoader(const std::string& path) {
        const std::filesystem::path cannonical_path = resolvePath(path);
        handler = dlopen(cannonical_path.c_str(), RTLD_NOW);

        if (!handler)
            throw std::runtime_error(std::format("Failed to open dynamic library {}:\n{}", cannonical_path.string(), dlerror()));

        dlerror();
    }


    ~UnixLoader() override {
        if (handler)
            dlclose(handler);
    }
};

std::unique_ptr<Base> createLoader(const std::string &path) {
    return std::make_unique<UnixLoader>(path);
}

} // loader
} // helpers