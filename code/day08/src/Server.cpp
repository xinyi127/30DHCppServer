#include "Server.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "Acceptor.h"
#include <functional>
#include <string.h>
#include <unistd.h>

#define READ_BUFFER 1024

// 创建 Acceptor 对象并绑定回调函数
Server::Server(EventLoop* _loop) : loop(_loop), acceptor(nullptr){
    acceptor = new Acceptor(loop);
    std::function<void(Socket*)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1); // _1 占位符，表示参数在调用时给出
    acceptor->setNewConnectionCallback(cb);
}

Server::~Server(){
    delete acceptor;
}

// 处理读事件，并向客户端写回相同数据
void Server::handleReadEvent(int sockfd){
    char buf[READ_BUFFER];
    while(true){
        memset(buf, 0, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        // 注意 if-else 逻辑对应，不要写混
        if(bytes_read == -1){ // 发生错误，需要根据 errno 判断错误类型
            if(errno == EINTR){ // EINTR 表示信号中断，说明客户端中断（注意不是断开连接），由于是边缘触发，需要继续读数据
                printf("conitnue reading");
                continue;
            } else if((errno == EAGAIN) || (errno == EWOULDBLOCK)){ // EAGAIN 和 EWOULDBLOCK 均表示资源暂时不可用，此时表明数据读取完成
                printf("finish reading once, errno: %d\n", errno);
                break;
            } 
        }else if(bytes_read == 0){ // 客户端断开连接
            printf("EOF, client fd %d disconnected\n", socket);
            close(sockfd); // 释放关闭连接的 socket，此时 socket 将自动从epoll树上移除
            break;
        } else if(bytes_read > 0){ // 正常读取到数据
            printf("message from client fd %d: %s\n", socket, buf);
            write(sockfd, buf, sizeof(buf)); // 回写到客户端
        }
    }
}

// 处理连接事件
void Server::newConnection(Socket* serv_sock){
    InetAddress* clnt_addr = new InetAddress(); // 注意，没有 delete，原因已在前几版说明
    Socket* clnt_sock = new Socket(serv_sock->accept(clnt_addr)); // 注意，没有 delete
    printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->getFd(), inet_ntoa(clnt_addr->addr.sin_addr), ntohs(clnt_addr->addr.sin_port));
    // 配置客户端 socket
    clnt_sock->setnonblocking();
    Channel* clntChannel = new Channel(loop, clnt_sock->getFd());
    std::function<void()> cb = std::bind(&Server::handleReadEvent, this, clnt_sock->getFd());
    clntChannel->setCallback(cb);
    clntChannel->enableReading();
}
