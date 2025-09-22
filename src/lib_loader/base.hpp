#pragma once

#include <filesystem>
#include <memory>
#include <string>

namespace lib_loader {

class Base {
protected:
    virtual void* getFunctionVoid(const std::string& name) const = 0;

public:
    virtual ~Base() = default;

    template<typename FuncSignature>
    FuncSignature getFunction(const std::string& name) const {
        return reinterpret_cast<FuncSignature>(getFunctionVoid(name));
    }
};

std::unique_ptr<Base> createLoader(const std::filesystem::path& path);

} // namespace lib_loader
