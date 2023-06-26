// 服务端初步抽象为 Reactor 模式，将 EventLoop 和 Channel 从服务端抽象出来，构成事件驱动模型。
// 抽象分离出 Aceptor 对象用于新建连接。而在实际的代码中，连接逻辑仍然放在 Server 类中，便于处理每个连接的特定需求，并且能够更好地管理和维护多个连接的状态。

#pragma once

class EventLoop;
class Socket;
class Acceptor;

class Server
{
private:
    EventLoop *loop;
    Acceptor* acceptor;
public:
     // 在构造函数中调用 Acceptor 以完成对监听 socket 的处理
    Server(EventLoop*);
    ~Server();

    void handleReadEvent(int); // 传入的是发生读事件的文件描述符
    void newConnection(Socket *serv_sock); // 处理客户端的连接请求
};
