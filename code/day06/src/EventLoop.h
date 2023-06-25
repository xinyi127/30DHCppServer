// 作为 Server(main Reactor) 的事件循环部分，负责监听并触发相应的回调函数
// 封装 Epoll 和 Channel 相关细节

#pragma once

class Epoll;
class Channel;

class EventLoop
{
private:
    Epoll *ep;
    bool quit; // 在 loop() 函数中会用到，恒为 false
public:
    EventLoop();
    ~EventLoop();

    void loop(); // 调用 loop() 开启事件启动
    void updateChannel(Channel*);
};
