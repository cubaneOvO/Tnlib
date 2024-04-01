#pragma once
#include "TcpServer.h"
#include "EventLoop.h"
#include "Connection.h"
#include "ThreadPool.h"
#include <functional>

class echoServer
{
private:
    TcpServer tcpServer_;
    ThreadPool threadpool_;

public:
    echoServer(const std::string&, const std::string&, int subthreadNum = 3, int workThreadNum = 5,
        int timeval = 30, //定时器触发时间间隔
        int timeout = 60,  //超时时间
        uint16_t sep = 1 //buffer处理的报文类型
    );
    ~echoServer();

    void start();
    void stop();

    //void HandleNewConnection(Socket*);//处理新用户连接，在TcpServer类中回调此函数
    //void HandleCloseConnection(spConnection);//关闭用户连接，在TcpServer类中回调此函数
    //void HandleError(spConnection);//处理连接错误，在TcpServer类中回调此函数
    void HandleMessage(spConnection, std::string&);//处理消息，在TcpServer类中回调此函数
    //void HandleSendFinish(spConnection);//消息发送完毕，在TcpServer类中回调此函数
    //void HandleEpollTimeout(EventLoop*);//epoll_wait超时，在TcpServer类中回调此函数

    void OnMassage(spConnection, std::string);//业务处理函数
};
