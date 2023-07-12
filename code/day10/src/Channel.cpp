#include "EventLoop.h"
#include "Channel.h"
#include <unistd.h>

// 初始化
Channel::Channel(EventLoop* _loop, int _fd) : loop(_loop), fd(_fd), events(0), revents(0), inEpoll(false){
}

// Channel 生命周期结束时也应该自动关闭 socket
Channel::~Channel(){
    if (fd != -1){
        close(fd);
        fd = -1;
    }
}

// callback() 根据 Channel 描述符的不同，分别绑定了连接和处理的函数
void Channel::handleEvent(){
    callback();
}

// 将 Channel 描述符添加到 epoll 树上并增加（第六版及以前是“设置”）监听事件为可读与边缘触发
void Channel::enableReading(){
    events |= EPOLLIN | EPOLLET;
    loop->updateChannel(this);
}

int Channel::getFd(){ 
    return fd; 
}

uint32_t Channel::getEvents(){ 
    return events; 
}

uint32_t Channel::getRevents() { 
    return revents; 
}

bool Channel::getInEpoll(){ 
    return inEpoll; 
}

void Channel::setInEpoll(){ 
    inEpoll = true; 
}

void Channel::setRevents(uint32_t _ev){ 
    revents = _ev; 
}

// 绑定 callback：在 Server.cpp 中创建新的 Channe 时根据描述符的不同，分别绑定到对应函数
void Channel::setCallback(std::function<void()> _cb){
    callback = _cb;
}
