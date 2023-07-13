// 作为 Server(main Reactor) 的事件循环部分，负责监听并触发相应的回调函数
// 进一步抽象封装 Epoll 的相关操作细节
//
// 当Channel类有事件需要处理时，将这个事件处理添加到线程池，
// 主线程EventLoop就可以继续进行事件循环，而不在乎某个socket fd上的事件处理

#pragma once
#include <functional>

class Epoll;
class Channel;
class ThreadPool;

class EventLoop
{
private:
    Epoll *ep;
    bool quit; // 用于 loop() 函数
    ThreadPool* threadPool;
public:
    EventLoop();
    ~EventLoop();

    void loop(); // 调用 loop() 开启事件启动
    void updateChannel(Channel*);

    void addThread(std::function<void()>); // 添加任务
};
