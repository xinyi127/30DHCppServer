// 对于 Acceptor，接受连接的处理时间较短、报文数据极小，并且一般不会有特别多的新连接在同一时间到达
// 所以 Acceptor 没有必要采用 epoll ET 模式，也没有必要用线程池。由于不会成为性能瓶颈，为了简单最好使用阻塞式 socket
// 具体更改如下
// 1. Acceptor socket fd（服务器监听 socket）使用阻塞式
// 2. Acceptor 使用 LT 模式，建立好连接后处理事件fd读写用ET模式
// 3. Acceptor 建立连接不使用线程池，建立好连接后处理事件用线程池

#include "Acceptor.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "Server.h"

Acceptor::Acceptor(EventLoop* _loop) : loop(_loop), sock(nullptr), acceptChannel(nullptr){
    // 创建监听 socket，绑定网络地址，监听
    sock = new Socket();
    InetAddress* addr = new InetAddress("127.0.0.1", 1234); // 0-1023 被用于标准服务，称为「知名端口」
    sock->bind(addr);
    sock->listen();
    // sock->setnonblocking(); // Acceptor 不采用 ET 模式，Socket 也就没有必要采用非阻塞模式

    // 为监听 socket 创建 Channel，为 Channel 绑定回调函数，设置监听事件并添加到 epoll 树上
    // 监听 socket：Channel.callback->Acceptor.acceptConnection->newConnectionCallback->Server.newConnection
    acceptChannel = new Channel(loop, sock->getFd());
    std::function<void()> cb = std::bind(&Acceptor::acceptConnection, this);
    acceptChannel->setReadCallback(cb);
    acceptChannel->enableRead();
    acceptChannel->setUseThreadPool(false); // 设置建立连接时不使用线程池
    delete addr; // 这一版中将 addr 从 Acceptor 的数据成员中移除，需要及时 delete
}

Acceptor::~Acceptor(){
    delete sock;
    delete acceptChannel;
}

// 接受连接：创建 socket 接受客户端连接，回调 Server 类中 newConnection() 函数新建 TCP 连接
void Acceptor::acceptConnection(){
    // InetAddress 在连接成功建立后被释放，Socket 在连接关闭后被释放
    InetAddress* clnt_addr = new InetAddress();
    Socket* clnt_sock = new Socket(sock->accept(clnt_addr));
    printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->getFd(), inet_ntoa(clnt_addr->getAddr().sin_addr), ntohs(clnt_addr->getAddr().sin_port));
    clnt_sock->setnonblocking();
    newConnectionCallback(clnt_sock); // 将 Socket 指针传给 Server 类的 Connecion 函数，作为 Connection 类的数据成员
    delete clnt_addr; // 关于 RAII：https://en.cppreference.com/w/cpp/language/raii
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket*)> _cb){
    newConnectionCallback = _cb;
}


