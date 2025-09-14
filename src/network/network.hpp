#pragma once

#ifndef NETWORK_H
#define NETWORK_H

#include <string>
#include "../application/renesmee.hpp"

namespace network {
    bool sendRequest(
        const InetAddr& addr,
        const Proxy& proxy,
        const std::string& login,
        const std::string& password
    );

    bool checkHikvision(const InetAddr& addr);
    std::string md5(const std::string& value);
}
    
#endif // NETWORK_H
