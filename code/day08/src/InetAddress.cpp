#include "InetAddress.h"
#include <string.h>

// 初始化列表，用来初始化类的成员变量和基类
InetAddress::InetAddress() : addr_len(sizeof(addr)) {
    memset(&addr, 0, sizeof(addr));
}

InetAddress::InetAddress(const char *ip, uint16_t port) : addr_len(sizeof(addr)) {
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    addr_len = sizeof(addr); // addr 的成员被修改，addr_len 的值也应该随之修改
}

InetAddress::~InetAddress() {
}

void InetAddress::setInetAddr(sockaddr_in _addr, socklen_t _addr_len){
    addr = _addr;
    addr_len = _addr_len;
}

sockaddr_in InetAddress::getAddr(){
    return addr;
}

socklen_t InetAddress::getAddr_len(){
    return addr_len;
}
