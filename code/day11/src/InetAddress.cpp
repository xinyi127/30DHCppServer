#include "InetAddress.h"
#include <string.h>

// 初始化列表，用来初始化类的成员变量和基类
InetAddress::InetAddress() {
    memset(&addr, 0, sizeof(addr));
}

InetAddress::InetAddress(const char* _ip, uint16_t _port) {
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(_ip);
    addr.sin_port = htons(_port);
}

InetAddress::~InetAddress() {
}

void InetAddress::setInetAddr(sockaddr_in _addr){
    addr = _addr;
}

sockaddr_in InetAddress::getAddr(){
    return addr;
}
