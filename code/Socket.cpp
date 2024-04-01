#include "Socket.h"

Socket::Socket(int fd):fd_(fd){
    assert(fd_ != -1);
}

Socket::~Socket(){
    if(fd_ >= 0)
        close(fd_);
}

int Socket::getFd() const{
    return fd_;
}

void Socket::setReuseaddr(bool on){
    int opt = 1;
    if(on)
        setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}

void Socket::setReuseport(bool on){
    int opt = 1;
    if(on)
        setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
}

void Socket::setTcpnodelay(bool on){
    int opt = 1;
    if(on)
        setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
}

void Socket::setKeepalive(bool on){
    int opt = 1;
        if(on)
     setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));
}

void Socket::Bind(const InterAddress& addr){
    int ret = bind(fd_, addr.getAddr(), sizeof(sockaddr));
        assert(ret != -1);
    setIpAndPort(addr.getIp(), addr.getPort());//对于监听fd而言在bind获取监听fd绑定的ip以及端口
}

void Socket::Listen(int n){
    int ret = listen(fd_, n);
        assert(ret != -1);
}

int Socket::Accept(InterAddress &clientaddr){
    socklen_t len = sizeof(sockaddr_in);
    int ret = accept4(fd_, clientaddr.getAddr(), &len, SOCK_NONBLOCK);//将连接sock设置为非阻塞
        assert(ret >= 0);
    return ret;   
}

int createNonblokingSock(){    
    int listenfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    assert(listenfd >= 0);
    return listenfd;
}

std::string Socket::getIp()const{
    return ip_;
}

uint16_t Socket::getPort()const{
    return port_;
}

void Socket::setIpAndPort(const char*ip, uint16_t port){
    ip_ = ip;
    port_ = port;
}