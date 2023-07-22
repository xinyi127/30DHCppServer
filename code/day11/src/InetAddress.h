// 封装 sockaddr_in IP地址绑定和类型转换的操作

#pragma once
#include <arpa/inet.h>

class InetAddress 
{
private:
    struct sockaddr_in addr;
public:
    InetAddress();
    InetAddress(const char* ip, uint16_t port);
    ~InetAddress();

    void setInetAddr(sockaddr_in _addr);
    sockaddr_in getAddr();
};
