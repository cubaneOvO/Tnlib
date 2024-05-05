#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>
#include <assert.h>
#include <fcntl.h>
#include <signal.h>
#include "InterAddress.h"
#include "Socket.h"
#include "Epoll.h"
#include "Channel.h"
#include "EventLoop.h"
#include "TcpServer.h"
#include "echoServer.h"

echoServer *echosev;

void Stop(int sig){//终止服务器程序
    printf("sig = %d\n", sig);
    printf("echoServer终止\n");
    echosev->stop();//终止Work/Io线程、事件循环
    delete echosev;
    exit(0);
}

int main(int argc, char const *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ip address> <port>\n", argv[0]);
        exit(1);
    }
    signal(SIGINT, Stop);
    signal(SIGTERM, Stop);
    echosev = new echoServer(argv[1], argv[2], 3, 5, 30, 60, 1);
    echosev->start();//事件循环
    return 0;
}
