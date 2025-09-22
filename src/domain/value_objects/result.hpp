#pragma once

#include "address.hpp"
#include "creds.hpp"

namespace domain::value_objects {

struct Result {
    Addr addr;
    Creds creds;
};

} // namespace domain::value_objects
