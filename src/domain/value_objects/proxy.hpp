#pragma once

#include "address.hpp"
#include "creds.hpp"

namespace domain::value_objects {

struct Proxy {
    Addr addr;
    Creds creds;
    std::string protocol;
};

} // namespace domain::value_objects
