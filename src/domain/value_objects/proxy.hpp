#pragma once

#include "address.hpp"
#include "api_DTOs.hpp"
#include "creds.hpp"

namespace domain::value_objects {

struct Proxy {
    Addr addr;
    Creds creds;
    std::string protocol;

    __Proxy to_c_struct() const noexcept {
        return {
            addr.ip.c_str(),
            addr.port,
            creds.login.c_str(),
            creds.password.c_str(),
            protocol.c_str(),
        };
    }
};

} // namespace domain::value_objects
