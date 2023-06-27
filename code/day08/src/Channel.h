// 封装添加到 epoll 树上的文件描述符有关事件的部分细节，一个 channel 类只负责一个文件描述符
// 记录文件描述符的额外信息，例如：需要监听的事件、被监听到的事件、发生事件后回调对应的函数，便于辅助 EventLoop 使用

#pragma once
#include <sys/epoll.h>
#include <functional> // std::function

class EventLoop;

class Channel
{
private:
    EventLoop* loop;
    int fd; // 文件描述符
    uint32_t events; // 监听 Channel 的哪些事件
    uint32_t revents; // epoll 返回 Channel 时文件描述符所发生的事件
    bool inEpoll; // 顾名思义，channel 是否在 epoll 红黑树中
    std::function<void()> callback; // 类模板，便于让 callback 对于不同的 Channel 指向不同的函数
public:
    // 构造和析构
    Channel(EventLoop *_loop, int _fd);
    ~Channel();

    // Channel 不在 epoll 红黑树中，则添加并更新监听事件；否则直接更新监听事件
    void enableReading();
    // 调用 callback()，注意此时 callback() 不一定指向 Server.h 中的 handleEvent() 函数，而是根据 Channel 描述符的不同
    // 分别绑定到 newConnection() 或 handleEvent() 函数上，前者负责连接，后者负责处理请求（读写）
    void handleEvent();

    // 获取/修改 数据成员
    int getFd();
    uint32_t getEvents();
    uint32_t getRevents();
    bool getInEpoll();

    void setInEpoll();
    void setRevents(uint32_t);
    void setCallback(std::function<void()>);
};


