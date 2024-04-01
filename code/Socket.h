#pragma once
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include "InterAddress.h"

int createNonblokingSock();//获取监听socket
class Socket{
private:
    const int fd_;
    std::string ip_;
    uint16_t port_;
public:
    Socket(int fd = -1);
    ~Socket(); //关闭fd_
    int getFd() const;
    void setIpAndPort(const char*ip, uint16_t port);
    void setReuseaddr(bool on);      // 设置SO_REUSEADDR选项，true-打开，false-关闭。
    void setReuseport(bool on);       // 设置SO_REUSEPORT选项。
    void setTcpnodelay(bool on);     // 设置TCP_NODELAY选项。
    void setKeepalive(bool on);       // 设置SO_KEEPALIVE选项。
    void Bind(const InterAddress&);
    void Listen(int n = 30);
    int Accept(InterAddress&);
    std::string getIp()const;
    uint16_t getPort()const;
};


