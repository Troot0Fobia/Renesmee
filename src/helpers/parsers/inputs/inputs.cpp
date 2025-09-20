#include "inputs.hpp"
#include <string>

namespace helpers {

    const std::pair<std::string, unsigned short> parseAddr(const std::string& addr) {
        std::string ip = addr;
        unsigned short port = 0;

        size_t pos;
        if ((pos = ip.find(':')) != std::string::npos && pos != ip.size() - 1) {
            int p = std::stoi(ip.substr(pos + 1));
            if (!isUShort(p)) port = 0;
            else port = static_cast<unsigned short>(p);
            ip = ip.substr(0, pos);
        }

        std::string ipCopy = ip;
        int octetCount = 0;

        for (; (pos = ip.find('.')) != std::string::npos && pos != ip.size() - 1; octetCount++) {
            int octet = std::stoi(ipCopy.substr(0, pos));
            if (octet < 0 || octet > 255)
                return {"", 0};
            ipCopy = ipCopy.substr(pos + 1);
        }

        if (!ipCopy.empty()) {
            int octet = std::stoi(ipCopy);
            if (octet < 0 || octet > 255)
                return {"", 0};

            octetCount++;
        }

        if (octetCount != 4)
            return {"", 0};

        return {ip, port};
    }

} // helpers