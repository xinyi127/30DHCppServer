#include "Server.h"
#include "Socket.h"
#include "Acceptor.h"
#include "Connection.h"
#include <functional>

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

// 新建 TCP 连接：创建 Connection 对象、绑定回调函数、加入 map
void Server::newConnection(Socket* sock){
    if(sock->getFd() != -1){
        Connection* conn = new Connection(loop, sock);
        std::function<void(int)> cb = std::bind(&Server::deleteConnection, this, std::placeholders::_1);
        conn->setDeleteConnectionCallback(cb);
        connections[sock->getFd()] = conn;
    }
}

// 将 TCP 连接从 map 上移除并删除该连接
void Server::deleteConnection(int sockfd){
    if(sockfd != -1){
        auto it = connections.find(sockfd);
        if(it != connections.end()){
            Connection* conn = connections[sockfd];
            connections.erase(sockfd);
            if(conn != nullptr)
                delete conn;
        }
    }
}
