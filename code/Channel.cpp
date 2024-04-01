#include "Channel.h"
    Channel::Channel(EventLoop* el, int fd):loop_(el), fd_(fd), inepoll_(false){

    }

    Channel::~Channel(){
        //不用处理loop_和fd，channel只是使用他们
    }

    int Channel::getFd(){
        return fd_;
    }

    void Channel::useET(){
        events_ |= EPOLLET;
    }

    void Channel::useOneshot(){
        events_ |= EPOLLONESHOT;
    }

    void Channel::enableReading(){
        events_ |= EPOLLIN;
        loop_->setChannel(this);
    }

    void Channel::disableReading(){
        events_ &= ~EPOLLIN;
        loop_->setChannel(this);
    }

    void Channel::enableWriting(){
        events_ |= EPOLLOUT;
        loop_->setChannel(this);
    }

    void Channel::disableWriting(){
        events_ &= ~EPOLLOUT;
        loop_->setChannel(this);
    }

    void Channel::disableAll(){
        events_ = 0;
        loop_->setChannel(this);
    }

    void Channel::remove(){
        loop_->removeChannel(this);//从红黑树上删除fd
    }

    void Channel::setInepoll(){
        inepoll_ = true;
    }

    void Channel::delInepoll(){
        inepoll_ = false;
    }

    void Channel::setRevents(uint32_t ev){
        revents_ = ev;
    }

    bool Channel::getInepoll()
    {
        return inepoll_;
    }

    uint32_t Channel::getEvents(){
        return events_;
    }

    uint32_t Channel::getRevents(){
        return revents_;
    }

    void Channel::handleEvent(){
        if(revents_ & EPOLLRDHUP){//表示对方已关闭链接，有些系统检测不到，
            closecallback_();
        }
        else if(revents_ & (EPOLLIN | EPOLLPRI)){
            readcallback_();//调用回调函数
        }
        else if(revents_ & EPOLLOUT){//有数据要写
            writecallback_(); 
        }  
        else{
            errorcallback_();
        }
    }

    void Channel::setReadcallback(std::function<void()> fn){
        readcallback_ = fn;
    }

    void Channel::setClosecallback(std::function<void()>fn){
        closecallback_ = fn;
    }
    void Channel::setErrorcallback(std::function<void()>fn){
        errorcallback_ = fn;
    }
    void Channel::setWritecallback(std::function<void()>fn){
        writecallback_ = fn;
    }
