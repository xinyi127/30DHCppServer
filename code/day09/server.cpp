// 整个服务器被抽象为 Server 类，简易 Reactor 模式。
// EventLoop 和 Channel 被抽象出来，构成事件驱动模型。
// Acceptor 负责接收连接。TCP 连接被抽象为 Connection，负责建立连接。

#include "src/EventLoop.h"
#include "src/Server.h"

int main() {
    // EventLoop 类从 Server 类中完全抽象出来，可被任意复用
    EventLoop* loop = new EventLoop();
    Server* server = new Server(loop);
    loop->loop();
    delete server;
    delete loop;
    return 0;
}
