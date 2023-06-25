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

    // 每次向 epoll 中添加文件描述符时，需要指定该文件描述符的事件类型
    void addFd(int fd, uint32_t op); // uint32_t 可以确保数据类型长度和位宽
    void updateChannel(Channel*);

    // timeout 默认为 -1
    //std::vector<epoll_event> poll(int timeout = -1);
    std::vector<Channel*> poll(int timeout = -1);
};
