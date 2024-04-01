#include "Acceptor.h"
Acceptor::Acceptor(EventLoop* loop, const std::string& ip, const std::string& port)
:loop_(loop), listenSock_(createNonblokingSock()), acceptChannel_(loop_, listenSock_.getFd())
{

    //重用本地地址，即使sock处于TIME_WAIT状态，与之绑定的sock也能立即被重用
    listenSock_.setReuseaddr(true);
    //禁止Nagle算法
    listenSock_.setTcpnodelay(true);
    //运输层KEEPALIVE（周期性保活机制）。注：最好是在应用层自己做心跳
    listenSock_.setKeepalive(true);
    //重用端口，在Reactor中意义不大。
    listenSock_.setReuseport(true);

    InterAddress addr(ip, port);//配置IPV4 socket地址结构体

    listenSock_.Bind(addr);
    listenSock_.Listen(128);

    acceptChannel_.setReadcallback(std::bind(&Acceptor::newConnection, this));
    acceptChannel_.enableReading();//令epoll监听fd上的读事件
}

Acceptor::~Acceptor(){
    
}

void Acceptor::newConnection(){//监听sock上有新连接
    InterAddress clientaddr;
    //此处需要释放内存，交给connection类处理
    std::unique_ptr<Socket> clientsock(new Socket(listenSock_.Accept(clientaddr)));
    clientsock->setIpAndPort(clientaddr.getIp(), clientaddr.getPort());
    newConnectionCB_(std::move(clientsock));
}

void Acceptor::setNewConnectionCB(std::function<void(std::unique_ptr<Socket>)> fn){
    newConnectionCB_ = fn;
}