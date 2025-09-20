#pragma once

#include <memory>
#include <string>
#ifndef BASE_LOADER_H
#define BASE_LOADER_H

namespace helpers {
namespace loader {

class Base {
protected:
    virtual void* getFunctionVoid(const std::string& name) const = 0;

public:
    virtual ~Base() = default;

    template<typename FuncSign>
    FuncSign getFunction(const std::string& name) const {
        return reinterpret_cast<FuncSign>(getFunctionVoid(name));
    }
};

std::unique_ptr<Base> createLoader(const std::string& path);

} // loader
} // helpers
    
#endif // BASE_LOADER_H
