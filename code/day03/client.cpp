#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "util.h"

#define BUFFER_SIZE 1024

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, (sizeof serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);
    // 第二个参数是指针类型，能够确保参数能够在不同协议族函数中正常使用。通过指针来转换类型的好处是：数据的内存空间没有改变，只是告诉编译器改变解释方式
    errif (connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket connect error");

    while(true) {
        char buf[BUFFER_SIZE];
        memset(buf, 0, sizeof(buf));
        scanf("%s", buf);
        ssize_t write_bytes = write(sockfd, buf, sizeof(buf));
        if (write_bytes == -1) { // 写数据失败。write 返回 -1 表示出错；返回 0 则比较特殊，通常发生在向一个非阻塞套接字写入数据时，表明写入 0 个字节数据
            printf("socket alreadly disconnected, can't write any more!\n");
            break;
        }

        memset(&buf, 0, sizeof(buf));
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
        if (read_bytes > 0) {
            printf("message from server: %s\n", buf);
        } else if (read_bytes == 0) { // read 函数返回 0 表示读到文件末尾或断开连接
            printf("server socket disconnneted!\n");
            break;
        } else if (read_bytes == -1) { // read 函数返回 -1 表示发生错误
            close(sockfd);
            errif(true, "socket read error");
        }
    }

    close(sockfd);
    return 0;
}
