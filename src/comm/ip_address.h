
#pragma once

#include <string>
#include <arpa/inet.h>

namespace pancake_store::comm {

class IPAddress {
public:
    static bool IsIPv4Address(const std::string& ipstr) {
        struct sockaddr_in sa;
        return inet_pton(AF_INET, ipstr.c_str(), &(sa.sin_addr)) != 0;
    }
};

} // namespace pancake_store::comm