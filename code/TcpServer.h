#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Acceptor.h"
#include "Connection.h"
#include "ThreadPool.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>

class TcpServer{
private:
    std::unique_ptr<EventLoop> mainloop_;//主事件循环
    std::vector<std::unique_ptr<EventLoop>> subloops_;//从事件循环
    Acceptor acceptor_;//一个server只能有一个acceprot对象
    std::unordered_map<int, spConnection> conns_;
    std::mutex mutex_;//给map加锁
    uint16_t sep_;//buffer处理的报文类型

    //要先声明threadnum,因为他会被用于初始化线程池大小
    int threadnum_;//线程池大小，即从事件循环的数量
    ThreadPool threadpool_;//线程池

    std::function<void(spConnection)> newConnectionCB_;
    std::function<void(spConnection)> closeconnectionCB_;
    std::function<void(spConnection)> errorconnectionCB_;
    std::function<void(spConnection, std::string&)> HandleMessageCB_;
    std::function<void(spConnection)> sendFinishCB_;
    std::function<void(EventLoop*)> epollTimeoutCB_;
public:
    TcpServer(const std::string&, const std::string&, int threadNum=3,
        int timeval = 30, //定时器触发时间间隔
        int timeout = 60,  //超时时间
        uint16_t sep = 1 //buffer处理的报文类型
    );
    ~TcpServer();
    void start();
    void stop();//停止IO线程和事件循环

    void newConnection(std::unique_ptr<Socket>);//处理新用户连接，在accept类中回调此函数
    void closeconnection(spConnection);//关闭用户连接，在connection类中回调此函数
    void errorconnection(spConnection);//处理连接错误，在connection类中回调此函数
    void HandleMessage(spConnection, std::string&);//处理消息，在connection类中回调此函数
    void sendFinish(spConnection);//消息发送完毕，在connection类中回调此函数
    void epollTimeout(EventLoop*);//epoll_wait超时，在EventLoop类中回调此函数
    void closeTimeoutConnection(int);

    //设置回调函数, 回调业务层的函数
    void setnewConnectionCB(std::function<void(spConnection)>);
    void setcloseconnectionCB(std::function<void(spConnection)>);
    void seterrorconnectionCB(std::function<void(spConnection)>);
    void setHandleMessageCB(std::function<void(spConnection, std::string&)>);
    void setsendFinishCB(std::function<void(spConnection)>);
    void setepollTimeoutCB(std::function<void(EventLoop*)>);
};
