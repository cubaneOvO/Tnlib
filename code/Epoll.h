#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>
#include <vector>
#include <unistd.h>
#include <assert.h>
#include "Channel.h"
#define MAX_EVENT_NUM 1024
class Channel;

class Epoll{
private:
    int epollfd_ = -1;
    epoll_event events_[MAX_EVENT_NUM];
public:
    Epoll(/* args */);
    ~Epoll();
    //void addfd(int, bool, bool);
    void setChannel(Channel*ch);
    void ermoveChannel(Channel*ch);
    std::vector<Channel*> loop(int timeout = -1);
};

