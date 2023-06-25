// 封装 sockaddr_in IP地址绑定和类型转换的操作

#pragma once
#include <arpa/inet.h>

class InetAddress 
{
public: // 为了便于使用，本应是 private 改为 public
    struct sockaddr_in addr;
    // socklen_t 是无符号整型
    socklen_t addr_len;
public:
    InetAddress();
    ~InetAddress();
    InetAddress(const char *ip, uint16_t port);
};
