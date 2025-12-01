#pragma once

#include "address.hpp"

namespace domain::value_objects {

struct BruteEntity {
    Addr address;
    size_t login_pos;
    size_t password_pos;
};

} // namespace domain::value_objects
