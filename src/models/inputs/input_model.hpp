#pragma once

#include <string>
#ifndef BASE_MODEL_H
#define BASE_MODEL_H

namespace models {

    struct Addr {
        std::string ip;
        unsigned short port;
    };

    struct Result {
        Addr addr;
        std::string login;
        std::string password;
    };

    struct Proxy {
        Result creds;
        std::string protocol;
    };

} // models

#endif // BASE_MODEL_H
