// 封装 sockaddr_in IP地址绑定和类型转换的操作

#pragma once
#include <arpa/inet.h>

class InetAddress 
{
private:
    struct sockaddr_in addr;
    // socklen_t 是无符号整型
    socklen_t addr_len;
public:
    InetAddress();
    InetAddress(const char* ip, uint16_t port);
    ~InetAddress();

    void setInetAddr(sockaddr_in _addr, socklen_t _addr_len);
    sockaddr_in getAddr();
    socklen_t getAddr_len();
};
