#include "TcpServer.h"

TcpServer::TcpServer(const std::string& ip, const std::string& port, int threadNum, int timeval, int timeout, uint16_t sep)
:threadnum_(threadNum), mainloop_(new EventLoop(true, timeval, timeout)), 
acceptor_(mainloop_.get(), ip, port), threadpool_(threadnum_, "I/O"), sep_(sep)
{
    mainloop_->setepollTimeoutCallback(std::bind(&TcpServer::epollTimeout, this, std::placeholders::_1));

    acceptor_.setNewConnectionCB(std::bind(&TcpServer::newConnection, this, std::placeholders::_1));

    for(int i = 0; i < threadnum_; i++){
        subloops_.emplace_back(new EventLoop(false, timeval, timeout));//直接在vector中构造，避免了拷贝
        subloops_[i]->setepollTimeoutCallback(std::bind(&TcpServer::epollTimeout, this, std::placeholders::_1));
        threadpool_.addtask(std::bind(&EventLoop::run, subloops_[i].get()));
    }
}

TcpServer::~TcpServer(){

}

void TcpServer::start(){
    mainloop_->run();
}

void TcpServer::stop(){
    //停止主事件循环
    mainloop_->stop();
    //停止从事件循环
    for(auto &loop : subloops_)
        loop->stop();
    //终止从事件循环线程
    threadpool_.stop();
}

void TcpServer::newConnection(std::unique_ptr<Socket> clientsock){//监听sock上有新连接
    //Connection* p = new Connection(mainloop_, clientsock);
    //将连接后的sock分配给从事件循环去监听
    int fd = clientsock->getFd();
    spConnection p(new Connection(subloops_[fd % threadnum_].get(), std::move(clientsock), sep_));
    {//加锁再往map中添加数据
        std::lock_guard<std::mutex> lock(mutex_);
        conns_[fd] = p;//添加到server的map容器中
    }
    subloops_[fd % threadnum_]->newConnection(p);//添加到Evevtloop的map容器中
    p->setClosecallback(std::bind(&TcpServer::closeconnection, this, std::placeholders::_1));
    p->seterrorCallback(std::bind(&TcpServer::errorconnection, this, std::placeholders::_1));
    p->setMessageCallback(std::bind(&TcpServer::HandleMessage, this, std::placeholders::_1, std::placeholders::_2));
    p->setsendfinshCallback(std::bind(&TcpServer::sendFinish, this, std::placeholders::_1));

    if(newConnectionCB_)//创建连接后回调
        newConnectionCB_(p);
}

void TcpServer::closeconnection(spConnection conn){
    if(closeconnectionCB_)//关闭连接前回调
        closeconnectionCB_(conn);
    {//加锁再删除数据
        std::lock_guard<std::mutex> lock(mutex_);
        conns_.erase(conn->getFd());
    }
}
    
void TcpServer::errorconnection(spConnection conn){
    if(closeconnectionCB_)//关闭连接前回调
        closeconnectionCB_(conn);
    {//加锁再删除数据
        std::lock_guard<std::mutex> lock(mutex_);
        conns_.erase(conn->getFd());
    }
}

void TcpServer::HandleMessage(spConnection conn, std::string& message){//message中存储着一个tcp数据包中的数据
    if(HandleMessageCB_)
        HandleMessageCB_(conn, message);
}

void TcpServer::sendFinish(spConnection conn){
    if(sendFinishCB_)
        sendFinishCB_(conn);
}

 void TcpServer::epollTimeout(EventLoop* loop){
    if(epollTimeoutCB_)
        epollTimeoutCB_(loop);
 }

void TcpServer::setnewConnectionCB(std::function<void(spConnection)> fn){
    newConnectionCB_ = fn;
}

void TcpServer::setcloseconnectionCB(std::function<void(spConnection)> fn){
    closeconnectionCB_ = fn;
}

void TcpServer::seterrorconnectionCB(std::function<void(spConnection)> fn){
    errorconnectionCB_ = fn;
}

void TcpServer::setHandleMessageCB(std::function<void(spConnection, std::string&)> fn){
    HandleMessageCB_ = fn;
}

void TcpServer::setsendFinishCB(std::function<void(spConnection)> fn){
    sendFinishCB_ = fn;
}

void TcpServer::setepollTimeoutCB(std::function<void(EventLoop*)> fn){
    epollTimeoutCB_ = fn;
}
