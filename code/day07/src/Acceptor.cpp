#include "Acceptor.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "Server.h"

Acceptor::Acceptor(EventLoop* _loop) : loop(_loop){
    // 创建监听 socket，绑定网络地址，监听，设置非阻塞
    sock = new Socket();
    addr = new InetAddress("127.0.0.1", 8888);
    sock->bind(addr);
    sock->listen();
    sock->setnonblocking();
    // 为监听 socket 创建 Channel，为 Channel 绑定回调函数，设置监听事件并添加到 epoll 树上
    // 监听 socket：Channel.callback->Acceptor.acceptConnection->newConnectionCallback->Server.newConnection
    acceptChannel = new Channel(loop, sock->getFd());
    std::function<void()> cb = std::bind(&Acceptor::acceptConnection, this);
}

Acceptor::~Acceptor(){
    delete sock;
    delete addr;
    delete acceptChannel;
}

void Acceptor::acceptConnection(){
    newConnectionCallback(sock);
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket*)> _cb){
    newConnectionCallback = _cb;
}


