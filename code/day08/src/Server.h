// 服务端初步抽象为 Reactor 模式，将 EventLoop 和 Channel 从服务端抽象出来，构成事件驱动模型。
// 添加 Aceptor 对象负责接收连接，将 TCP 连接抽象为 Connection 类。将所有 TCP 连接保存在 map 中，便于 Server 管理。

#pragma once

#include <map>

class EventLoop;
class Socket;
class Acceptor;
class Connection;

class Server
{
private:
    EventLoop *loop; // 事件循环
    Acceptor* acceptor; // 用于接收 TCP 连接
    // key 为连接客户端的 socket，value 为指向该连接（Connection）的指
    std::map<int, Connection*> connections; // 管理所有 TCP 连接
public:
     // 在构造函数中调用 Acceptor 以完成对监听 socket 的处理
    Server(EventLoop*);
    ~Server();

    // 客户端 socket 发生事件：Channel->handleEvent() 调用 Connection->echo()
    // void handleReadEvent(int); // 传入的是发生读事件的文件描述符 
    void newConnection(Socket *serv_sock); // 新建 TCP 连接
    void deleteConnection(Socket* sock); // 断开 TCP 连接
};
