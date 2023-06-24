#include "Epoll.h"
#include "Channel.h"

// 初始化
Channel::Channel(Epoll *_ep, int _fd) : ep(_ep), fd(_fd), events(0), revents(0), inEpoll(false){
}

Channel::~Channel(){
}

// 让 ep 监听 fd 的可读与边缘触发事件
void Channel::enableReading(){
    events = EPOLLIN | EPOLLET;
    ep->updateChannel(this);
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

