// Reactor 模式中，将 Acceptor 抽象出来用于接受连接请求
// 由于Acceptor面对的是多种连接，其实际的连接代码仍将连接逻辑放在 Server 中
// 将连接逻辑放在Server类中进行处理，也可以更方便地扩展和管理多个连接

#pragma once

#include <functional>

class EventLoop;
class Socket;
class Channel;

class Acceptor
{
private:
    EventLoop* loop;
    Socket* sock; // 每个 Acceptor 对应一个服务端监听 socket
    Channel* acceptChannel;
    std::function<void(Socket*)> newConnectionCallback;
public:
    Acceptor(EventLoop* _loop);
    ~Acceptor();
    void acceptConnection();
    void setNewConnectionCallback(std::function<void(Socket*)>);
};
