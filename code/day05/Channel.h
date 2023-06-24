// 封装文件描述符有关事件的部分，一个 channel 类只负责一个文件描述符

#pragma once
#include <sys/epoll.h>

class Epoll; // 将 Epoll 指针作为 channel 的成员

class Channel
{
private:
    Epoll *ep; // 
    int fd; // 文件描述符
    uint32_t events; // 监听 fd 的哪些事件
    uint32_t revents; // epoll 返回 channel 时文件描述符正在发生的事件
    bool inEpoll; // 顾名思义，channel 是否在 epoll 红黑树中
public:
    // 构造和析构
    Channel(Epoll *_ep, int _fd);
    ~Channel();

    // Channel 不在 epoll 红黑树中，则添加；否则更新 Channel、打开允许读事件
    void enableReading();

    // 获取/修改 数据成员
    int getFd();
    uint32_t getEvents();
    uint32_t getRevents();
    bool getInEpoll();
    void setInEpoll();
    void setRevents(uint32_t);
};


