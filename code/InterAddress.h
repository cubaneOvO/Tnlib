#pragma once
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <string>

class InterAddress{
private:
    sockaddr_in addr_;
public:
    InterAddress(const std::string&, const std::string&);
    InterAddress() = default;
    ~InterAddress() = default;
    const char* getIp() const;
    uint16_t getPort() const;
    sockaddr* getAddr() const;
};


