# Tnlib
`Tnlib`是一个基于主从Reactor模式的 C++ 网络库，支持TCP协议，在Linux环境中运行
## ✨ 特性
- 使用非阻塞socket+epoll（LT/ET模式）支持监听处理多个连接/请求事件。
- 使用线程池管理I/O线程和Work线程。
- 为数据读写设置了Buffer，在inputbuffer中对TCP粘包拆包问题进行处理。
- 创建和操作定时器，通过文件描述符通知定时器到期事件，检查并断开长时间未通讯的tcp连接。
-  Work线程将写操作封装成任务并通过任务队列异步提交到I/O线程。

## ⌛️ 构建
通过Makefile
```
make
```

## 📚 示例
### echoserver
#### 服务端
main: [code/Server.cpp](code/Server.cpp)

业务实现: [code/echoServer.cpp](code/echoServer.cpp)

#### 客户端
[code/client.cpp](code/client.cpp)



