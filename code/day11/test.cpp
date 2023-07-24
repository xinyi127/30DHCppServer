#include <iostream>
#include <unistd.h>
#include <string.h>
#include <functional>
#include "src/util.h"
#include "src/Buffer.h"
#include "src/InetAddress.h"
#include "src/Socket.h"
#include "src/ThreadPool.h"

using namespace std;

void oneClient(int msgs, int wait){
    Socket* sock = new Socket();
    InetAddress* addr = new InetAddress("127.0.0.1", 1234);
    sock->connect(addr);

    int sockfd = sock->getFd();

    Buffer* sendBuffer = new Buffer();
    Buffer* readBuffer = new Buffer();

    sleep(wait); // 等待 wait 秒
    int count = 0;
    while(count < msgs){
        sendBuffer->setBuf("I'm client!");
        ssize_t write_bytes = write(sockfd, sendBuffer->c_str(), sendBuffer->size());
        if(write_bytes == -1){
            printf("socket already disconnected, can't write any more!\n");
            break;
        }
        int already_read = 0;
        char buf[1024];
        while(true){
            memset(buf, 0, sizeof(buf));
            ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
            if(read_bytes > 0){
                readBuffer->append(buf, read_bytes);
                already_read += read_bytes;
            } else if(read_bytes == 0){
                printf("server disconnected!\n");
                exit(EXIT_SUCCESS);
            }
            if(already_read >= sendBuffer->size()){
                printf("count: %d, message from server: %s\n", count++, readBuffer->c_str());
                break;
            }
        }
        readBuffer->clear();
    }
    delete addr;
    delete sock;
}

// 参数个数、参数（二维数组）
int main(int argc, char* argv[]){
    int threads = 100; // 线程数
    int msgs = 100; // 消息树
    int wait = 0; // 等待时间
    int o;
    const char* optstring = "t:m:w:";
    while((o = getopt(argc, argv, optstring)) != -1){
        // optarg 是全局变量
        switch (o){
            case 't':
                threads = stoi(optarg);
                break;
            case 'm':
                msgs = stoi(optarg);
                break;
            case 'w':
                wait = stoi(optarg);
                break;
            // 遇到未指定选项或缺少参数
            case '?':
                printf("error optopt: %c\n", optopt);
                printf("error opterr: %d\n", opterr);
                break;
        }
    }

    ThreadPool* poll = new ThreadPool(threads);
    // 将 oneClient 函数与消息数 msgs 和等待时间 wait 绑定为一个可调用对象 func
    // 从而将 oneClient 函数作为任务提交给线程池执行时，能够携带特定的参数
    std::function<void()> func = std::bind(oneClient, msgs, wait);
    for(int i = 0; i < threads; ++ i){
        poll->add(func);
    }
    delete poll;
    return 0;
}
