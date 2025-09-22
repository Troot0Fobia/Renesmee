#pragma once

#include <string>

namespace domain::value_objects {

struct Addr {
    std::string ip;
    unsigned short port;
};

} // namespace domain::value_objects
