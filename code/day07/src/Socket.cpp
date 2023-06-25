#include "Socket.h"
#include "InetAddress.h"
#include "util.h"
#include <unistd.h> // close()
#include <fcntl.h> // fcntl()
#include <sys/socket.h>

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

void Socket::bind(InetAddress *addr) {
    // ::bind() 前的作用域限定符用于表明这是全局 bind() 函数
    errif(::bind(fd, (sockaddr*)&addr->addr, addr->addr_len) == -1, "socket bind error"); // 注意 & 和 ->
}

void Socket::listen() {
    // SOMAXCONN 参数的最大值由 Linux 内核参数 /proc/sys/net/core/somaxconn 决定，一般为 128
    errif(::listen(fd, SOMAXCONN) == -1, "socket listen error");
}

void Socket::setnonblocking() {
    // fcntl() 函数有多种不同的重载形式
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int Socket::accept(InetAddress *addr) {
    // accept() 和 bind() 函数的参数类型相同
    int clnt_socket = ::accept(fd, (sockaddr*)&addr->addr, &addr->addr_len); // 注意 accept 第三个参数是指针类型
    errif(clnt_socket == -1, "socket accept error");
    return clnt_socket;
}

int Socket::getFd() {
    return fd;
}





