#include "Connection.h"

Connection::Connection(EventLoop* loop, std::unique_ptr<Socket> cilentSock, uint16_t sep)
:loop_(loop), cilentSock_(std::move(cilentSock)), inputbuffer_(sep), outputbuffer_(sep),
disConnect(false),connectChannel_(new Channel(loop_, cilentSock_->getFd()))
{
    connectChannel_->setReadcallback(std::bind(&Connection::onMessage, this));
    connectChannel_->setClosecallback(std::bind(&Connection::closeCallback, this));
    connectChannel_->setErrorcallback(std::bind(&Connection::errorCallback, this));
    connectChannel_->setWritecallback(std::bind(&Connection::sendMessage, this));
    connectChannel_->useET();//设置边缘触发
    connectChannel_->enableReading();//监听连接sock上的读事件
}

Connection::~Connection(){
    
}

int Connection::getFd() const{
    return cilentSock_->getFd();
}

std::string Connection::getIp()const{
    return cilentSock_->getIp();
}

uint16_t Connection::getPort()const{
    return cilentSock_->getPort();
}

void Connection::closeCallback(){//tcp连接关闭时的回调函数
    disConnect = true;
    connectChannel_->remove();//将channel从epoll红黑树上删除
    closeCallback_(shared_from_this());
}
void Connection::errorCallback(){//tcp连接出错时的回调函数
    disConnect = true;
    connectChannel_->remove();//将channel从epoll红黑树上删除
    errorCallback_(shared_from_this());// 关闭客户端的fd。
}

void Connection::setClosecallback(std::function<void(spConnection)>fn){
    closeCallback_ = fn;
}
    
void Connection::seterrorCallback(std::function<void(spConnection)>fn){
    errorCallback_ = fn;
}

void Connection::setMessageCallback(std::function<void(spConnection, std::string&)>fn){
    messageCallback_ = fn;
}

void Connection::setsendfinshCallback(std::function<void(spConnection)> fn){
    sendfinshCallback_ = fn;
}

void Connection::onMessage(){//连接sock上有新数据
    char buffer[BUFFER_SIZE];
    while(1){
        ssize_t nread = recv(getFd(), buffer, sizeof(buffer), 0);
        if(nread == 0){//客户端关闭了链接
            disConnect = true;
            closeCallback();
            break;
        }
        else if(nread < 0){
            if(errno == EINTR)//被信号中断
                continue;
            else if(errno == EAGAIN || errno == EWOULDBLOCK){//buffer中数据已经读完
                std::string message;
                while(true){//解决粘包分包问题
                    if(inputbuffer_.pickMessage(message) == false)
                        break;
                    lastTime_ = TimeStarmp::now();//更新时间戳

                    messageCallback_(shared_from_this(), message);
                }

                break;
            }
        }
        else{
            inputbuffer_.append(buffer, nread);//暂存于缓冲区中

        }
    }
}

void Connection::send(std::string& message, size_t size){
    if(disConnect) return;
    if(loop_->isLoopThread()){//是IO线程
        regSend(message, size);
    }
    else{//不是IO线程
        loop_->queueInLoop(std::bind(&Connection::regSend, this, message, size));//将“注册写事件”任务加入IO线程的任务队列中
    }
}


//注：若regSend与sendMessage不在同一线程中则有可能造成竞争
void Connection::regSend(std::string message, size_t size){
    outputbuffer_.appendWithsep(message.data(), size);
    connectChannel_->enableWriting();//向epoll注册写事件
}

void Connection::sendMessage(){
    if(disConnect) return;
    int writen = ::send(getFd(), outputbuffer_.data(), outputbuffer_.size(), 0);//调用函数前加"::"，代表调用的是全局函数，不是类自己的成员函数。
    if(writen > 0)outputbuffer_.erase(0, writen);
    
    if(outputbuffer_.size() == 0){//发送缓冲区中没数据了，数据传输完毕
        connectChannel_->disableWriting();//不再监听写事件
        sendfinshCallback_(shared_from_this());
    }
}

bool Connection::timeout(time_t now, int val){
    return now - lastTime_.toInt() > val;
}

