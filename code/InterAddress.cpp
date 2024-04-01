#include "InterAddress.h"
InterAddress::InterAddress(const std::string& ip, const std::string& port){
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());
    addr_.sin_port = htons(atoi(port.c_str()));
}
const char* InterAddress::getIp() const{
    return inet_ntoa(addr_.sin_addr);
}
uint16_t InterAddress::getPort() const{
    return ntohs(addr_.sin_port);
}
sockaddr* InterAddress::getAddr() const{
    return (sockaddr*)&addr_;
}