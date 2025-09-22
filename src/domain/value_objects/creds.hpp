#pragma once

#include <string>

namespace domain::value_objects {

struct Creds {
    std::string login;
    std::string password;
};

} // namespace domain::value_objects
