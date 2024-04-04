#pragma once
#include "Epoll.h"
#include <functional>
#include <sys/syscall.h>
#include <memory>
#include <queue>
#include <mutex>
#include <atomic>
#include <sys/eventfd.h>
#include <sys/timerfd.h>//定时器头文件
#include <unordered_map>
#include "Connection.h"

class Channel;
class Epoll;
class Connection;
using spConnection = std::shared_ptr<Connection>;//指定类型别名

class EventLoop
{
private:
    std::unique_ptr<Epoll> ep_;
    std::function<void(EventLoop*)>epollTimeoutCallback_;//处理epoll_wait超时的回调函数
    pid_t threadId_;//存储从事件循环线程的id，用于区从事件循环线程与Work线程
    bool IsMainLoop_;//用于区分主事件循环与从事件循环
    std::atomic_bool stop_;

    //任务队列，存放work线程处理后的发送任务，避免了两个不同线程共同处理发送时的竞争问题
    std::queue<std::function<void()>> taskqueue_;
    std::mutex taskMutex_;
    int wakeupFd_;
    std::unique_ptr<Channel> wakeChannel_;

    //定时器
    int timeval_;//定时器触发时间间隔
    int timeout_;//超时时间
    int timeFd_;//定时器fd
    std::unique_ptr<Channel> timeChannel_;


    //存放所有connection
    std::mutex mapMutex_;//给map加锁
    std::unordered_map<int, spConnection> conns_;
    
    std::queue<std::pair<std::function<void(spConnection)>, spConnection>> closequeue_;//关闭队列
public:
    EventLoop(bool, int, int);
    ~EventLoop();
    void run();//运行事件循环
    void stop();//停止事件循环
    void setChannel(Channel*);//向epoll注册事件
    void setepollTimeoutCallback(std::function<void(EventLoop*)>);
    void removeChannel(Channel*);//从epoll中删除事件
    
    bool isLoopThread();//判断当前线程是否为事件循环线程

    void queueInLoop(std::function<void()>);//入队函数
    void wakeup();//用eventfd唤醒事件循环
    void handleWakeup();

    void handleTimeout();
    void newConnection(spConnection);//向unordered_map中加入元素
    void setclosetimeoutCB(std::function<void(int)>);
        
    void closequeueInLoop(std::function<void(spConnection)>, spConnection);//关闭连接入队函数
};

