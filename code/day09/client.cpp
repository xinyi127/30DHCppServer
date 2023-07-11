#include <iostream>
#include <string.h>
#include <unistd.h>
#include "src/util.h"
#include "src/Buffer.h"
#include "src/InetAddress.h"
#include "src/Socket.h"

int main(){
    // 创建 socket、网络地址、连接服务器
    Socket* sock = new Socket();
    InetAddress* addr = new InetAddress("127.0.0.1", 1234);
    sock->connect(addr);

    int sockfd = sock->getFd();

    // 发送和接收缓冲区
    Buffer* sendBuffer = new Buffer();
    Buffer* readBuffer = new Buffer();

    while(true){
        // 向服务器端发送请求
        sendBuffer->getline();
        ssize_t write_bytes = write(sockfd, sendBuffer->c_str(), sendBuffer->size());
        if(write_bytes == -1) { // 写数据时出错
            printf("socket already disconnected, can't write any more!\n");
            break;
        }

        int already_read = 0; // 读取完数据的标志
        char buf[1024]; // 大小无所谓，会全部存到读缓冲区。原本是边写边收，现在是全部收完再写
        while(true){
            memset(buf, 0, sizeof(buf));
            ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
            if(read_bytes > 0){
                readBuffer->append(buf, read_bytes);
                already_read += read_bytes;
            } // 如果已经读取完数据，会在输出后退出循环；所以此处是 socket 连接被关闭 
              else if(read_bytes == 0){
                printf("server disconnected!\n");
                exit(EXIT_SUCCESS); // exit() 函数用于终止程序执行并退出
            } else if(read_bytes == -1){ // 出错
                errif(-1, "socket read error");
                exit(EXIT_FAILURE);
            }

            // 网络中数据是分片传输的，用 >= 判断可以增强鲁棒性
            if(already_read >= sendBuffer->size()){
                printf("message from server: %s\n", readBuffer->c_str());
                break;
            }
        }
        readBuffer->clear();
    }
    delete addr;
    delete sock;
    return 0;
}
