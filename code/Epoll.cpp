#include "Epoll.h"
Epoll::Epoll(/* args */){//创建epoll句柄
    epollfd_ = epoll_create(5);
    assert(epollfd_ != -1);
}

Epoll::~Epoll(){
    close(epollfd_);
}

/*void Epoll::addfd(int fd, bool ETmod, bool oneshot){//将事件注册到epoll内核事件表中
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    if(ETmod)
        event.events |= EPOLLET;
    if(oneshot)
        event.events |= EPOLLONESHOT;
    epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &event);
}*/

void Epoll::setChannel(Channel*ch){
    epoll_event event;
    event.data.ptr = ch;//指定channel
    event.events = ch->getEvents();//指定要监听的事件
    if(ch->getInepoll()){//判断channel是否已经在epoll树中
        int ret = epoll_ctl(epollfd_, EPOLL_CTL_MOD, ch->getFd(), &event);//更改监听的事件
        assert(ret != -1);
    }
    else{
        int ret = epoll_ctl(epollfd_, EPOLL_CTL_ADD, ch->getFd(), &event);
        assert(ret != -1);
        ch->setInepoll();
    }
}

std::vector<Channel*> Epoll::loop(int timeout){
    std :: vector<Channel*> vec;
    //memset(events_, 0, sizeof(events_));
    int infds = epoll_wait(epollfd_, events_, MAX_EVENT_NUM, timeout);
    assert(infds >= 0);

    for(int i = 0; i < infds; i++){
        Channel* ch = (Channel*)events_[i].data.ptr;
        ch->setRevents(events_[i].events);//设置发生的事件
        vec.push_back(ch);
    }
    return vec;
}

void Epoll::ermoveChannel(Channel*ch){
    if(ch->getInepoll()){//判断channel是否已经在epoll树中
        int ret = epoll_ctl(epollfd_, EPOLL_CTL_DEL, ch->getFd(), 0);//删除channel
        assert(ret != -1);
        ch->delInepoll();//修改标志，表示channel已经从树种删除
    }
}