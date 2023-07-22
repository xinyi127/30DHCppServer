#include "EventLoop.h"
#include "Channel.h"
#include "Socket.h"
#include <unistd.h>
#include <sys/epoll.h>

// 初始化
Channel::Channel(EventLoop* _loop, int _fd)
    : loop(_loop), fd(_fd), events(0), ready(0), inEpoll(false), useThreadPool(true){
}

// Channel 生命周期结束时也应该自动关闭 socket
Channel::~Channel(){
    if (fd != -1){
        close(fd);
        fd = -1;
    }
}

// 根据发生的事件和是否使用线程池进行决策
void Channel::handleEvent(){
    if(ready & (EPOLLIN | EPOLLPRI)){
        if(useThreadPool)
            loop->addThread(readCallback);
        else 
            readCallback();
    }
    if(ready & EPOLLOUT){
        if(useThreadPool)
            loop->addThread(writeCallback);
        else 
            writeCallback();
    }
}

// 设置监听事件为可读和高优先级可读事件，将 Channel 描述符添加到 epoll 树上
void Channel::enableRead(){
    events |= EPOLLIN | EPOLLPRI;
    loop->updateChannel(this);
}

// 设置事件处理模式为 ET 边缘触发模式
void Channel::useET(){
    events |= EPOLLET;
    loop->updateChannel(this);
}

int Channel::getFd(){
    return fd;
}

uint32_t Channel::getEvents(){
    return events;
}

uint32_t Channel::getReady() {
    return ready;
}

bool Channel::getInEpoll(){
    return inEpoll; 
}

void Channel::setInEpoll(bool _in){
    inEpoll = _in;
}

void Channel::setReady(uint32_t _rd){
    ready = _rd;
}

// 绑定 callback：在 Server.cpp 中创建新的 Channe 时根据描述符的不同，分别绑定到对应函数
//void Channel::setCallback(std::function<void()> _cb){
//    callback = _cb;
//}

void Channel::setReadCallback(std::function<void()> _cb){
    readCallback = _cb;
}

void Channel::setUseThreadPool(bool use){
    useThreadPool = use;
}
