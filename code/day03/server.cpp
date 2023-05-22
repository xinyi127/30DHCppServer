#include "util.h" // errif
#include <sys/socket.h> // socket 及相关函数和数据结构
#include <arpa/inet.h> // IP 地址转换相关的函数和宏，该头文件引入了 <netinet/in.h>
#include <sys/epoll.h> // 创建 epoll 实例，向实例中添加、修改或删除监视的文件描述符，等待 I/O 事件的发生等
#include <fcntl.h> // 与文件描述符相关的常量、类型和函数，例如 fcntl()
#include <string.h>
#include <stdio.h>
#include <unistd.h> // close() 函数
#include <errno.h> // errno

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

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

    while(true) {
        // 返回发生事件的文件描述符的个数,这些文件描述符都是已经注册到 epoll 实例中的文件描述符
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1); // 第四个参数表示等待时间， -1 表示永久等待，0 表示立即返回
        errif(nfds == -1, "epoll wait error");
        // 发生的事件信息均被存在 events 中，共 nfds 个，遍历所有事件
        for (int i = 0; i < nfds; i ++) {
            if (events[i].data.fd == sockfd) { // 发生该事件的文件描述符是 sockfd，说明有新的客户端连接请求
                struct sockaddr_in clnt_addr;
                bzero(&clnt_addr, sizeof(clnt_addr));
                socklen_t clnt_addr_len = sizeof(clnt_addr);

                // 调用 accept 会阻塞，直到有客户端连接到服务器
                int clnt_sockfd = accept(sockfd, (sockaddr*)&clnt_addr, &clnt_addr_len); // 注意 accept 原型的后两个参数均为指针
                errif(clnt_sockfd == -1, "socket accept error");
                // inet_ntoa 不需要指定 sin_addr 具体的成员，而 inet_addr 则需要
                printf("new client fd %d! IP: %s, Port: %d\n", clnt_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

                // 将 clnt_sockfd 添加到 epoll，并指明监听的事件类型
                bzero(&ev, sizeof(ev));
                ev.data.fd = clnt_sockfd;
                // 对于客户端连接，使用 ET 模式，可以让 epoll 更加高效，支持更多并发，但实际上接受连接最好不要用 ET 模式
                ev.events = EPOLLIN | EPOLLET;
                setnonblocking(clnt_sockfd); // ET 模式需搭配非阻塞式 socket 使用，非阻塞式 socket 在进行 I/O 操作时，如果没有数据，则立即返回
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sockfd, &ev);
            } else if (events[i].events & EPOLLIN) { // 可读事件，epoll_event 通过 events 的低 7 位记录 7 种不同事件类型
                char buf[READ_BUFFER]; // 定义缓冲区
                while(true) {
                    bzero(&buf, sizeof(buf));
                    ssize_t bytes_read = read(events[i].data.fd, buf, sizeof(buf)); // ssize_t 是有符号整数，经常用于系统调用函数的返回类型
                    if (bytes_read == -1) {
                        // errno 是 C 标准库中一个全局的、线程安全的变量，用于表示最近一次系统调用，声明在头文件 errno.h 中
                        if (errno == EINTR) { // errno 值为 EINTR 表示操作被信号中断，此处表示客户端正常中断，需要继续读取
                            printf("continue reading");
                            continue;
                        } else if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) { // EAGAIN 和 EWOULDBLOCK 均表示资源暂时不可用，在非阻塞 I/O 下表示数据全部被读完
                            printf("finish reading once, errno: %d\n", errno);
                            break;
                        }
                    } else if (bytes_read == 0) { // read 返回 0 表示 EOF，此处表示客户端断开连接
                        printf("EOF, client fd %d disconnected\n", events[i].data.fd);
                        epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL); // 在关闭 socket 之前，应当先将其从 epoll 中删除
                        close(events[i].data.fd); // socket 被关闭后，是否会自动从 epoll 中删除：https://blog.csdn.net/qq_43684922/article/details/99205294
                        break;
                    } else if (bytes_read > 0) {
                        printf("message from client fd %d: %s\n", events[i].data.fd, buf);
                        write(events[i].data.fd, buf, sizeof(buf));
                    }
                }
            } else {
                printf("something else happened\n"); // 发生其他事件，留待后续实现
            }
        }
    }

    epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL);
    close(sockfd);
    return 0;
}

// 开始调用 epoll_wait 时，只有一个文件描述符需要监听，也就是 sockfd，此后当客户端有连接请求时，将客户端的 socket 添加到 epoll，
// 此时需要监听多个文件描述符，事实上在本程序里，sockfd 只会发生 EPOLLET 事件，而客户端的 socket 只会发生 EPOLLIN 事件。
