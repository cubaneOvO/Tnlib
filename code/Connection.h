#pragma once
#include <functional>
#include "Socket.h"
#include "InterAddress.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Buffer.h"
#include <memory>
#include <atomic>
#include "TimeStarmp.h"

class EventLoop;
class Channel;
class Connection;//前置声明

using spConnection = std::shared_ptr<Connection>;//指定类型别名

class Connection : public std::enable_shared_from_this<Connection>
{
private:
    EventLoop* loop_;//connect对应的事件循环，在构造函数中传入
    std::unique_ptr<Socket> cilentSock_;//服务端用于监听的socket，在构造函数中传入(并由它来管理socket的生命周期)
    std::unique_ptr<Channel> connectChannel_;//acceptor对应的channel，在构造函数中创建
    Buffer inputbuffer_;//接收缓冲区
    Buffer outputbuffer_;//发生缓冲区
    std::atomic_bool disConnect;//如果为true则标志tcp连接已经断开

    std::function<void(spConnection)> closeCallback_;
    std::function<void(spConnection)> errorCallback_; 
    std::function<void(spConnection, std::string&)> messageCallback_;
    std::function<void(spConnection)> sendfinshCallback_;

    TimeStarmp lastTime_;
public:
    Connection(EventLoop* loop, std::unique_ptr<Socket>clientsock, uint16_t sep);
    ~Connection();
    int getFd() const;
    std::string getIp()const;
    uint16_t getPort()const;
    void send(std::string&, size_t);//根据线程类型执行不同操作
    void regSend(std::string, size_t);//向epoll注册写事件
    bool timeout(time_t, int);//判断tcp连接是否超时

    void closeCallback();//tcp连接关闭时的回调函数
    void errorCallback();//tcp连接出错时的回调函数
    void onMessage();//接收数据的回调函数
    void sendMessage();//发送数据的回调函数

    void setClosecallback(std::function<void(spConnection)>);
    void seterrorCallback(std::function<void(spConnection)>);
    void setMessageCallback(std::function<void(spConnection, std::string&)>);
    void setsendfinshCallback(std::function<void(spConnection)>);
};


