#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <vector>
#include "util.h"
#include "Epoll.h"
#include "InetAddress.h"
#include "Socket.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void setnonblocking(int fd){
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK); // FL 是 flag 的缩写
}

void handleReadEvent(int, int);

int main() {
    // 建立 socket、绑定网络地址、监听 socket
    Socket *serv_sock = new Socket();
    InetAddress *serv_addr = new InetAddress("127.0.0.1", 8888);
    serv_sock->bind(serv_addr);
    serv_sock->listen();

    // 将服务器 socket 设置为非阻塞式
    serv_sock->setnonblocking();
    Epoll *ep = new Epoll();
    // 将服务器 sockfd 添加到 epoll 树上
    ep->addFd(serv_sock->getFd(), EPOLLIN | EPOLLET);

    while(true){
        // poll() 成员函数中实现了 epoll_wait()，返回 epoll 树上发生的事件信息
        std::vector<epoll_event> events = ep->poll();
        int nfds = events.size();
        for(int i = 0; i < nfds; ++ i){
            // 发生事件的文件描述符是服务器 socket，说明有新的客户端连接请求
            if(events[i].data.fd == serv_sock->getFd()){
                // 这里 new 的对象后续没有 delete，可能会发生内存泄露。
                // 为什么 clnt_sock 和 Socket 不及时delete？前者是因为加入了 epoll 中，后续还要对其进行操作；
                // 后者是因为如果直接 delete，则可能会破坏该 Socket 对象所持有的地址信息的状态，所以在前者没有 delete 前，后者必须保证处于有效状态。
                InetAddress *clnt_addr = new InetAddress(); //*3
                Socket *clnt_sock = new Socket(serv_sock->accept(clnt_addr));
                printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->getFd(), inet_ntoa(clnt_addr->addr.sin_addr), ntohs(clnt_addr->addr.sin_port));
                clnt_sock->setnonblocking(); // ET 模式需要搭配非阻塞模式 socket 使用
                // 将客户端 socket 加入到 epoll 树上，并设置监听的事件
                ep->addFd(clnt_sock->getFd(), EPOLLIN | EPOLLET);
            } else if(events[i].events & EPOLLIN){ // 可读事件
                handleReadEvent(events[i].data.fd, ep->getFd());
            } else{
                printf("something else happened");
            }
        }
    }

    delete serv_sock;
    delete serv_addr;
    delete ep; //*1
    return 0;
}

void handleReadEvent(int socket, int epfd){
    char buf[READ_BUFFER]; // 定义缓冲区
    while(true) {
        memset(buf, 0, sizeof(buf));
        ssize_t bytes_read = read(socket, buf, sizeof(buf));
        if (bytes_read == -1) { // read() 返回 -1，表明发生错误，需要根据 errno 的值具体判断是什么错误
            if (errno == EINTR) { // errno 值为 EINTR 表示操作被信号中断，此处表示客户端正常中断，需要继续读取
                printf("continue reading");
                continue;
            } else if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) { // EAGAIN 和 EWOULDBLOCK 均表示资源暂时不可用，在非阻塞 I/O 下表示数据全部被读完
                printf("finish reading once, errno: %d\n", errno);
                break;
            }
        } else if (bytes_read == 0) { // read 返回 0 表示 EOF，此处表示客户端断开连接
            printf("EOF, client fd %d disconnected\n", socket);
            epoll_ctl(epfd, EPOLL_CTL_DEL, socket, NULL); // 在关闭 socket 之前，应当先将其从 epoll 中删除 //*2
            close(socket);
            break;
        } else if (bytes_read > 0) {
            printf("message from client fd %d: %s\n", socket, buf);
            write(socket, buf, sizeof(buf));
        }
    }
    return ;
}
