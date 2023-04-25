#include "util.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

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

    return 0;
}
