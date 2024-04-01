#include "EventLoop.h"

int createTimefd(int sec = 30){//创建timeFd
    int tfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);//创建timefd
    struct itimerspec timeout;
    memset(&timeout, 0, sizeof(struct itimerspec));
    timeout.it_value.tv_sec = sec;
    timeout.it_value.tv_nsec = 0;
    timerfd_settime(tfd, 0, &timeout, 0);
    return tfd;
}

EventLoop::EventLoop(bool IsMainLoop, int timeval, int timeout)
:ep_(new Epoll), wakeupFd_(eventfd(0, EFD_NONBLOCK)), wakeChannel_(new Channel(this, wakeupFd_)),
stop_(false), timeval_(timeval), timeout_(timeout),
timeFd_(createTimefd(timeval_)), timeChannel_(new Channel(this, timeFd_)), IsMainLoop_(IsMainLoop)
{
    wakeChannel_->setReadcallback(std::bind(&EventLoop::handleWakeup, this));
    wakeChannel_->enableReading();

    if(!IsMainLoop_){//只有从时间循环猜需要往epoll注册
        timeChannel_->setReadcallback(std::bind(&EventLoop::handleTimeout, this));
        timeChannel_->enableReading();
    }
}

EventLoop::~EventLoop(){

}

void EventLoop::run(){
    threadId_ = syscall(SYS_gettid);//获取事件循环所在线程的id
    //printf("Eventloop::run() thread is %d\n",syscall(SYS_gettid));
    std::vector<Channel*> vec;
    while(!stop_){//事件循环
        vec = ep_->loop();
        if(vec.size() == 0)//调用回调函数
            epollTimeoutCallback_(this);
        for(auto& ch: vec)
            ch->handleEvent();
    }
}

void EventLoop::stop(){
    stop_ = true;
    wakeup();//唤醒epoll_wait
}

void EventLoop::setChannel(Channel* ch){
    ep_->setChannel(ch);
}

void EventLoop::removeChannel(Channel* ch){
    ep_->ermoveChannel(ch);
}

void EventLoop::setepollTimeoutCallback(std::function<void(EventLoop*)> fn){
    epollTimeoutCallback_ = fn;
}

bool EventLoop::isLoopThread(){
    return (threadId_ == syscall(SYS_gettid));//判断当前线程是否是事件循环线程
}

void EventLoop::queueInLoop(std::function<void()> fn){
    {
        std::lock_guard<std::mutex> lock(this->taskMutex_);//给任务队列加锁
        taskqueue_.push(fn);
    }
    wakeup();//唤醒IO线程
}

void EventLoop::wakeup(){
    uint64_t val = 1;
    write(wakeupFd_, &val, sizeof(val));//随便写点东西去唤醒IO线程
}

void EventLoop::handleWakeup(){
    uint64_t val;
    int ret = read(wakeupFd_, &val, sizeof(val));//尝试读取判断是否有发送任务
    if(ret != -1){
        std::function<void()> fn;
        std::lock_guard<std::mutex> lock(this->taskMutex_);//给任务队列加锁
        while(taskqueue_.size() > 0){//执行队列中全部任务
            fn = std::move(taskqueue_.front());
            taskqueue_.pop();
            fn();
        }
    }
}

void EventLoop::handleTimeout(){

    if(IsMainLoop_){

    }
    else{//从事件循环
        time_t now = time(0);
        for(auto i = conns_.begin(); i != conns_.end();){
            if(i->second->timeout(now, timeout_)){//超时了
                i->second->closeCallback();
                {
                    std::lock_guard<std::mutex> lock(mapMutex_);
                    conns_.erase(i++);
                }
            }
            else i++;
        }
    }

    //重新开始计时
    struct itimerspec timeout;
    memset(&timeout, 0, sizeof(struct itimerspec));
    timeout.it_value.tv_sec = timeval_;
    timeout.it_value.tv_nsec = 0;
    timerfd_settime(timeFd_, 0, &timeout, 0);
}

void EventLoop::newConnection(spConnection conn){
    {
        std::lock_guard<std::mutex> lock(mapMutex_);
        conns_[conn->getFd()] = conn;
    }
}

void EventLoop::setclosetimeoutCB(std::function<void(int)> fn){
    closetimeoutCB_ = fn;
}