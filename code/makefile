all:client echoserver

client:client.cpp
	g++ -O -o client client.cpp

echoserver:Server.cpp InterAddress.cpp Socket.cpp Epoll.cpp Channel.cpp EventLoop.cpp TcpServer.cpp Acceptor.cpp Connection.cpp Buffer.cpp echoServer.cpp ThreadPool.cpp TimeStarmp.cpp
	g++ -O -g -o echoserver Server.cpp InterAddress.cpp Socket.cpp Epoll.cpp Channel.cpp EventLoop.cpp TcpServer.cpp Acceptor.cpp Connection.cpp Buffer.cpp echoServer.cpp ThreadPool.cpp TimeStarmp.cpp -lpthread

clean:
	rm -f client echoserver
