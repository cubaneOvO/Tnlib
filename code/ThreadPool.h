#pragma once
#include <mutex>
#include <unistd.h>
#include <thread>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>
#include <vector>
#include <queue>
#include <sys/syscall.h>


class ThreadPool
{
private:
    std::vector<std::thread> threads_;//线程
    std::queue<std::function<void()>> taskqueue_;//任务队列
    std::mutex mutex_; //任务队列同步的互斥锁
    std::condition_variable condition_; //任务队列的条件变量
    std::atomic_bool stop_; //在析构函数中把stop_的值设置为true，全部的线程将退出

    std::string threadType_;
public:
    ThreadPool(size_t, const std::string);//在构造函数中启动Num个线程
    ~ThreadPool();

    size_t size();
    void addtask(std::function<void()>);//向任务队列中添加任务
    void stop();//停止所有线程
};

