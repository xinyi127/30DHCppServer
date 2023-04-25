#include "util.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    // connect 函数参数包括一个套接字描述符，一个指向目标主机地址和端口号的结构体指针，和该结构体的大小。
    // connect函数要进行TCP三次握手，如果成功则返回0，如果失败则返回-1。connect函数的失败是通过超时来控制的，它会在规定的时间内发起多次连接。
    int recnt = connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr));
    errif(recnt == -1, "socket connect error");

    while(true) {
        // 定义缓冲区，存放从键盘输入到服务器的数据。
        char buf[1024];
        // 清空缓冲区。
        bzero(&buf, sizeof(buf));
        printf("请输入数据：\n");
        scanf("%s", buf);
        // 发送缓冲区中的数据到服务器socket，返回已发送数据大小。
        ssize_t write_bytes = write(sockfd, buf, sizeof(buf));
        // write() 函数会将缓冲区 buf 中的 sizeof(buf) 个字节写入文件 sockfd，成功则返回写入的字节数，失败则返回 -1。
        if(write_bytes == -1){
            printf("socket already disconncted, can't write any more!\n");
            break;
        }
        // 清空缓冲区。
        bzero(&buf, sizeof(buf));
        // 从服务器 socket 读到缓冲区，返回已读数据大小。
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
        // 在 read() 中，<0 代表出错（实际上只用到了 -1），==0 代表 EOF，>0 才是读取成功，此时代表字节数。关于 read() 返回 0：https://www.zhihu.com/question/355020251
        if(read_bytes > 0) {
            printf("message from server: %s\n", buf);
        } else if(read_bytes == 0) { // read 返回 0，表示 EOF，通常是服务器断开连接，等会儿可以通过人为终止服务器连接进行测试
            printf("server socket disconnected!\n");
            break;
        } else if(read_bytes == -1) { // read 返回 -1，表示发生错误，按照上文方法进行错误处理。
            // 发生错误，等下调用错误处理函数后会自动退出程序，所以需要提前关闭 sockfd。
            close(sockfd);
            errif(true, "socket read error");
        }
    }
    close(sockfd);
    return 0;
}

// 两台计算机之间的通信相当于两个套接字之间的通信.
// 在服务器端用 write() 向套接字写入数据，客户端就能收到，然后再使用 read() 从套接字中读取出来，就完成了一次通信。
