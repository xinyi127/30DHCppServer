#include "Socket.h"
#include "InetAddress.h"
#include "util.h"
#include <unistd.h> // close()
#include <fcntl.h> // fcntl()
#include <sys/socket.h>
#include <string.h>

// 尽管在进入 Socket() 构造函数后会第一时间为 fd 赋值，但初始化成员变量是个好习惯
Socket::Socket() : fd(-1) {
    fd = socket(AF_INET, SOCK_STREAM, 0);
    errif(fd == -1, "socket create error");
}

Socket::Socket(int _fd) : fd(_fd) {
    errif(fd == -1, "socket create error");
}

Socket::~Socket() {
    // fd 不等于 -1，说明该对象并没有创建对应的 socket 文件描述符
    if(fd != -1) {
        close(fd);
        fd == -1;
    }
}

// 引入中间变量，提高代码的可读性、可维护性和可扩展性
// 可以在函数中进行更多的逻辑处理或者对传入的参数进行修改，而不会直接改变用户传入的 InetAddress 对象
void Socket::bind(InetAddress *_addr) {
    // ::bind() 前的作用域限定符用于表明这是全局 bind() 函数
    struct sockaddr_in addr = _addr->getAddr();
    socklen_t addr_len = _addr->getAddr_len();
    errif(::bind(fd, (sockaddr*)&addr, addr_len) == -1, "socket bind error"); // 注意 &
    _addr->setInetAddr(addr, addr_len);
}

void Socket::listen() {
    // SOMAXCONN 参数的最大值由 Linux 内核参数 /proc/sys/net/core/somaxconn 决定，一般为 128
    errif(::listen(fd, SOMAXCONN) == -1, "socket listen error");
}

void Socket::setnonblocking() {
    // fcntl() 函数有多种不同的重载形式
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int Socket::accept(InetAddress* _addr) {
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    memset(&addr, 0, sizeof(addr));
    // accept() 和 bind() 函数的参数类型相同
    int clnt_sockfd = ::accept(fd, (sockaddr*)&addr, &addr_len); // 注意 accept 第三个参数也是指针类型
    errif(clnt_sockfd == -1, "socket accept error");
    _addr->setInetAddr(addr, addr_len);
    return clnt_sockfd;
}

int Socket::getFd() {
    return fd;
}





