#pragma once
#include <sys/epoll.h>
#include <sys/socket.h>
#include <functional>
#include "InterAddress.h"
#include "Socket.h"
#include "EventLoop.h"
#include <memory>
#include <atomic>
#define BUFFER_SIZE 1024

class EventLoop;

class Channel{
private:
    int fd_ = -1; //channel拥有的fd，1对1关系
    //通过eventloop类channel可找到关联的epoll，一个epoll对应多个channel
    EventLoop* loop_;//channel关联的事件循环，在构造函数中传入,没有所有权
    std::atomic_bool inepoll_; //标志Channel是否已经添加到epoll树上
    uint32_t events_ = 0; //fd_需要监视的事件
    uint32_t revents_ = 0; //fd_已经发生的事件
    std::function<void()> readcallback_;//读事件回调函数
    std::function<void()> closecallback_;
    std::function<void()> errorcallback_;
    std::function<void()> writecallback_;
public:
    Channel(EventLoop* el, int fd);
    ~Channel();
    int getFd(); //返回fd成员
    void useET(); //采用边缘触发
    void useOneshot();//设置oneshot标志
    void enableReading(); //注册fd_读事件
    void disableReading();//取消读事件
    void enableWriting();//注册fd_写事件
    void disableWriting();//取消写事件
    void disableAll();//取消所有事件
    void remove();//将channel从epoll中删除
    void setInepoll();//标志Channel已经添加到epoll树上
    void delInepoll();//将Channel从epoll树上删除
    void setRevents(uint32_t ev);
    bool getInepoll();//返回inepoll成员
    uint32_t getEvents();//返回events成员
    uint32_t getRevents();//返回revents
    void handleEvent();//处理事件

    void setReadcallback(std::function<void()>);//设置回调函数
    void setClosecallback(std::function<void()>);//设置回调函数
    void setErrorcallback(std::function<void()>);//设置回调函数
    void setWritecallback(std::function<void()>);//设置回调函数
    void delFunc();//清空回调函数
};


