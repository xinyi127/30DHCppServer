// EventLoop 的 loop() 通过 Epoll 的 poll() 函数得到发生事件的 Channel
// 回调 Channel 绑定的回调函数以处理事件
//
// 整个服务器被抽象为 Server，简易 Reactor 模式。
// EventLoop 和 Channel 被抽象出来，构成事件驱动模型。

#include "src/EventLoop.h"
#include "src/Server.h"

int main() {
    // EventLoop 类从 Server 类中完全抽象出来，可被任意复用
    EventLoop* loop = new EventLoop();
    Server* server = new Server(loop);
    loop->loop();
    return 0;
}
