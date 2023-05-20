#include "util.h" // errif
#include <sys/socket.h> // socket 及相关函数和数据结构
#include <arpa/inet.h> // IP 地址转换相关的函数和宏，该头文件引入了 <netinet/in.h>
#include <sys/epoll.h> // 创建 epoll 实例，向实例中添加、修改或删除监视的文件描述符，等待 I/O 事件的发生等
#include <fcntl.h> // 与文件描述符相关的常量、类型和函数，例如 fcntl
#include <string.h>

#define MAX_EVENTS 1024

// 将文件描述符 fd 设置为非阻塞模式
void setnonblocking(int fd) {
    // 首先使用 fcntl(fd, F_GETFL) 获取文件 fd 的状态标记，然后将 O_NONBLOCK 标记加入状态标记值中
    // 再将新的状态标记用 F_SETFL 操作设置为该文件的状态标记，从而将文件描述符设置为非阻塞模式
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");
    // sockaddr_in 专用于 IPv4，sockaddr_in6 专用于 IPv6，sockaddr 通用地址结构
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    // 将点分十进制IP地址转换为网络字节序无符号长整型IP地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888); // 主机字节序到网络字节序
    // 将一个未连接的socket与特定的 IP 地址和端口号相关联。第二个参数本质上是一种"将派生类指针转换为基类指针"的操作
    errif(bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket bind error");
    // 监听 sockfd 绑定的对应 IP 地址和端口
    errif(listen(sockfd, SOMAXCONN) == -1, "socket listen error");

    // 创建一个 epoll 实例，并返回一个文件描述符
    int epfd = epoll_create1(0);
    errif(epfd == -1, "epoll create error");

    struct epoll_event events[MAX_EVENTS], ev;
    bzero(&events, sizeof(events));

    bzero(&ev, sizeof(ev));
    ev.data.fd = sockfd; // data 字段是一个联合体，用于保存用户数据
    // events 字段是 32 位无符号整数，用于保存事件类型，该类型可以是多种标志的组合 
    ev.events = EPOLLIN | EPOLLET; // 可读数据 | 边缘触发
    setnonblocking(sockfd);
    // 关于 epoll_ctl 函数：https://www.cnblogs.com/dream-chaser/p/7401184.html
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev); // 添加事件到 epoll


    return 0;
}
