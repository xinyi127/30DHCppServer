// 将 TCP 连接抽象成 Connection 类，由 Server 直接管理，由 Channel 分发到 epoll，通过回调函数处理事件。
// Connection 类专注于 TCP 连接，所以将对应的事件处理逻辑封装到 Connection 类自己中。

#pragma once

#include <functional>

class EventLoop;
class Socket;
class Channel;
class Connection
{
private:
    EventLoop* loop;
    Socket* sock; // 连接客户端的 socket
    Channel* channel;
    // 由于 Connection 的生命周期由 Server 进行管理，所以也应该由 Server 来删除连接。
    std::function<void(Socket*)> deleteConnectionCallback; // 在 Server 新建每个连接时绑定其删除回调函数
public:
    Connection(EventLoop* _loop, Socket* _sock);
    ~Connection();

    void echo(int sockfd);  // 向客户端回写数据
    void setDeleteConnectionCallback(std::function<void(Socket*)>);
};
