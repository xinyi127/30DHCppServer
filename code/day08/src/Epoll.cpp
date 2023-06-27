#include "Epoll.h" // 包含了 <vector>
#include "Channel.h"
#include "util.h"
#include <unistd.h>
#include <string.h>
#include <new> // new 在 C++ 中是内置运算符，不加头文件也可以使用，但为了可读性和可维护性，此处加上 <new>

#define MAX_EVENTS 1024

// 相较于 NULL，nullptr 被专门用来表示空指针
Epoll::Epoll() : epfd(-1), events(nullptr) {
    // epoll_create1() 创建的 epoll 能够监听的文件描述符的数量由内核自行分配
    // 系统对每个用户的 epoll 文件描述符的最大监听数量限制：/proc/sys/fs/epoll/max_user_watches
    epfd = epoll_create1(0);
    errif(epfd == -1, "epoll create error");
    events = new epoll_event[MAX_EVENTS];
    // bzero() 在 C++11 标准中已经被正式弃用，此后涉及 bzero() 的地方将一律用 memset() 代替。
    memset(events, 0, sizeof(*events) * MAX_EVENTS); // 注意 events 数组的大小计算方式，sizeof(*events) 等价于 sizeof(epoll_event)
}

Epoll::~Epoll() {
    if(epfd == -1){
        close(epfd);
        epfd = -1;
    }
    // detele 后加下标运算符 []，表明删除数组类型的对象。
    // delete[] 与 new[] 配对使用。原因是：如果不加 [] 只会调用第一个对象的析构函数。
    delete [] events;
}

//int Epoll::getFd(){
//    return epfd;
//}

//void Epoll::addFd(int fd, uint32_t op) {
//    struct epoll_event ev;
//    memset(&ev, 0, sizeof(ev));
//    ev.data.fd = fd;
//    ev.events = op;
//    // epoll_ctl() 第二个参数只有三种取值：EPOLL_CTL_ADD、EPOLL_CTL_MOD、EPOLL_CTL_DEL
//    errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add event error");
//}

// 在该函数后重新实现新的版本
//std::vector<epoll_event> Epoll::poll(int timeout) {
//    std::vector<epoll_event> activeEvents;
//    int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
//    errif(nfds == -1, "epoll wait error");
//    // 将存在 events[] 中的事件转存到 activeEvents 中
//    for(int i = 0; i < nfds; ++ i) {
//        activeEvents.push_back(events[i]);
//    }
//    return activeEvents;
//}

// 重新实现 Epoll::poll() 函数，文件描述符被封装为 Channel，用 Channel 的事件数据成员来保存发生的事件 
std::vector<Channel*> Epoll::poll(int timeout){ // timeout 传入的默认值为 -1
    std::vector<Channel*> activeChannels;
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    errif(nfds == -1, "epoll wait error");
    for(int i = 0; i < nfds; ++ i){
        // epoll_event.data.ptr 字段用于存储事件相关的用户自定义数据指针，将待定的上下文或数据与事件关联起来
        Channel *ch = (Channel*)events[i].data.ptr;
        ch->setRevents(events[i].events);
        activeChannels.push_back(ch);
    }
    return activeChannels;
}

// 如果 channel 中封装的文件描述符尚未被添加 epoll 红黑树则添加到树上并更新事件，否则直接更新事件。
void Epoll::updateChannel(Channel *channel){
    int fd = channel->getFd();
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    // 将 channel 与该事件关联
    ev.data.ptr = channel;
    // 在 Channel::enableReading() 中已经设置好事件类型
    ev.events = channel->getEvents();
    if(!channel->getInEpoll()){
        errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add error");
        // 务必记得更新 channel 的 inEpoll 状态
        channel->setInEpoll();
    } else{
        errif(epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll modify error");
    }
}


