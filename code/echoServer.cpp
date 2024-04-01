#include "echoServer.h"

echoServer::echoServer(const std::string& ip, const std::string& port, int subthreadNum, int workThreadNum,
int timeval, int timeout, uint16_t sep)
:tcpServer_(ip, port, subthreadNum, timeval, timeout, sep), threadpool_(workThreadNum, "Work")
{
    //echo业务层只关心接收到的消息
    tcpServer_.setHandleMessageCB(std::bind(&echoServer::HandleMessage, this, std::placeholders::_1, std::placeholders::_2));
    
}

echoServer::~echoServer(){

}

void echoServer::start(){
    tcpServer_.start();
}

void echoServer::stop(){
    threadpool_.stop();//停止所有工作线程
    tcpServer_.stop();
}

/*void echoServer::HandleNewConnection(Socket*){
    std::cout << "New Connection" << std::endl;
    //相关业务逻辑
}   

void echoServer::HandleCloseConnection(spConnection){
    std::cout << "Close Connection" << std::endl;
    //相关业务逻辑
}

void echoServer::HandleError(spConnection){
    std::cout << "Error" << std::endl;
    //相关业务逻辑
}*/

void echoServer::HandleMessage(spConnection conn, std::string& message){
    if(threadpool_.size() == 0){//处理简单逻辑时，不使用工作线程
        OnMassage(conn, message);
    }
    else{
        //把业务添加到工作线程池队列中
        
        threadpool_.addtask(std::bind(&echoServer::OnMassage, this, conn, message));
    }
}
/*void echoServer::HandleSendFinish(spConnection){
    std::cout << "Send Finish" << std::endl;
    //相关业务逻辑
}

void echoServer::HandleEpollTimeout(EventLoop*){
    std::cout << "Epoll Timeout" << std::endl;
    //相关业务逻辑
}*/

void echoServer::OnMassage(spConnection conn, std::string message){//这里不能是引用，因为生命周期不一样
    ////
    //处理业务逻辑
    ////
    conn->send(message, message.size());//将回复报文加入输出缓冲区中
}
