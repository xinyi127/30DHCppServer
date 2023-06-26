// Reactor 模式中的 Acceptor 对象，负责获取连接
// Reactor 对象通过 EventLoop 监听事件，如果是连接建立的事件，则交由 Acceptor 对象进行处理

#pragma once

#include <functional>

class EventLoop;
class Socket;
class InetAddress;
class Channel;

class Acceptor
{
private:
    EventLoop* loop;
    Socket* sock; // 每个 Acceptor 对应一个服务端监听 socket
    InetAddress* addr;
    Channel* acceptChannel;
public:
    Acceptor(EventLoop* _loop);
    ~Acceptor();
    void acceptConnection();
    // newConnetionCallback 是一个函数封装对象，实际绑定的函数在 Server 类中实现
    std::function<void(Socket*)> newConnectionCallback;
    void setNewConnectionCallback(std::function<void(Socket*)>);
};
