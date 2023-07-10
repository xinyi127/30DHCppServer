#include "Connection.h"
#include "Socket.h"
#include "Channel.h"
#include "util.h"
#include "Buffer.h"
#include <unistd.h>
#include <string.h>
#include <iostream>

// 为新的 TCP 连接创建 Channel，绑定回调函数，打开读事件监听
Connection::Connection(EventLoop* _loop, Socket* _sock) : loop(_loop), sock(_sock), channel(nullptr), inBuffer(new std::string()), readBuffer(nullptr){
    channel = new Channel(loop, sock->getFd());
    std::function<void()> cb = std::bind(&Connection::echo, this, sock->getFd());
    channel->setCallback(cb);
    channel->enableReading();
    readBuffer = new Buffer();
}

// 析构函数只负责将某个 TCP 连接的 Channel 和 Socket 类释放，真正删除一个 TCP 连接还需要将其 Connection 实例从 map 中移除
// 将 Connection 类从 map 中移除操作，通过回调函数在 Server 中实现
Connection::~Connection(){
    delete channel;
    delete sock;
}

void Connection::echo(int sockfd){
    char buf[1024]; // 此处 buf 的大小无所谓，因为是先将数据全部读到缓冲区后，再一次发送给服务器端
    while(true){
        memset(buf, 0, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        // 注意 if-else 逻辑对应，不要写混
        if(bytes_read == -1){ // 发生错误，需要根据 errno 判断错误类型
            if(errno == EINTR){ // EINTR 表示信号中断，说明客户端中断（注意不是断开连接），由于是边缘触发，需要继续读数据
                printf("conitnue reading");
                continue;
            } else if((errno == EAGAIN) || (errno == EWOULDBLOCK)){ // EAGAIN 和 EWOULDBLOCK 均表示资源暂时不可用，此时表明数据读取完成
                printf("finish reading once\n");
                printf("message from client fd %d: %s\n", sockfd, readBuffer->c_str());
                errif(write(sockfd, readBuffer->c_str(), readBuffer->size()) == -1, "socket wirte error");
                break;
            } 
        }else if(bytes_read == 0){ // 客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            // close(sockfd); // 释放关闭连接的 socket，此时 socket 将自动从epoll树上移除
            deleteConnectionCallback(sock);
        } else if(bytes_read > 0){ // 正常读取到数据
            readBuffer->append(buf, bytes_read);
        }
    }
}

void Connection::setDeleteConnectionCallback(std::function<void(Socket*)> _cb){
    deleteConnectionCallback = _cb;
}

