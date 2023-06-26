// 将服务端初步抽象为 Reactor 模式，将 EventLoop 和 Channel 从服务端抽象出来，构成事件驱动模型。
// 目前服务端只有一个 Reactor，而且只有一个线程。Redis 正是单 Reactor 单进程模式。
// 后续更新将修改为主从 Reactor 并添加 Acceptor 和线程池。

#pragma once

class EventLoop;
class Socket;

class Server
{
private:
    EventLoop *loop;
public:
    Server(EventLoop*); // 在构造函数中完成对监听 socket 的处理：包括加入 epoll 树、绑定 callback 等等
    ~Server();

    void handleReadEvent(int); // 传入的是发生读事件的文件描述符
    void newConnection(Socket *serv_sock); // 处理客户端的连接请求
};
