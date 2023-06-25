#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"
#include <vector>

EventLoop::EventLoop() : ep(nullptr), quit(false){
    // ep 改为类自己创建，而不是由用户创建，进一步封装细节
    ep = new Epoll();
}

EventLoop::~EventLoop() {
    delete ep;
}

void EventLoop::loop(){
    // !quit 恒为 true
    while(!quit){
        std::vector<Channel*> chs;
        chs = ep->poll();
        for(auto it = chs.begin(); it != chs.end(); ++ it){
            (*it)->handleEvent();
        }
    }
}

// 调用 Epoll 中的 updateChannel() 函数
void EventLoop::updateChannel(Channel* ch){
    ep->updateChannel(ch);
}
