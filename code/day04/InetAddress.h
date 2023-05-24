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
    ~InetAddress();
    InetAddress(const char *ip, uint16_t port);
}
