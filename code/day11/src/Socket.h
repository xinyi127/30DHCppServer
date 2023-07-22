// 封装 socket 有关的操作，例如创建 socket、绑定、监听、连接、设置非阻塞模式等

#pragma once

class InetAddress;

class Socket
{
private:
    int fd;
public:
    Socket();
    ~Socket();
    Socket(int _fd);

    void bind(InetAddress*);
    void listen();
    int accept(InetAddress*);

    void connect(InetAddress*); // 用于客户端连接服务器

    void setnonblocking();
    int getFd();
};
