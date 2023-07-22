// 封装 epoll 操作

#pragma once
#include <sys/epoll.h>
#include <vector>

// 后续 updateChannel() 函数会用到
class Channel;

class Epoll
{
private:
    int epfd;
    struct epoll_event *events; // 注意是指针，指向一个数组，用于存储发生的事件
public:
    Epoll();
    ~Epoll();

    // addFd() 函数完全由 updateChannel() 函数取代
    // void addFd(int fd, uint32_t op); // uint32_t 可以确保数据类型长度和位宽
    void updateChannel(Channel*); // 若不在 epoll 树上则加入并更新事件，否则直接更新事件
    void deleteChannel(Channel*); // 从 epoll 树上删除 Channel

    // timeout 默认为 -1
    //std::vector<epoll_event> poll(int timeout = -1);
    std::vector<Channel*> poll(int timeout = -1);
};
