#pragma once

class InetAddress;

class Socket
{
private:
    int fd;
public:
    Socket();
    ~Socket();
    Socket(int);

    void bind(InetAddress*);
    void listen();
    void setnonblocking();

    int accept(InetAddress*);

    int getFd();
};
