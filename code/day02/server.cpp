#include "util.h" // 自定义的错误处理函数 util
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>

int main() {
    // fd 是 file descriptor 文件描述符。更多关于 socket fd ：https://zhuanlan.zhihu.com/p/399651675。
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // socket返回的值是一个文件描述符，0、1、2 分别表示标准输入、标准输出、标准错误，所以其他打开的文件描述符都会大于 2, 错误时就返回 -1。
    errif(sockfd == -1, "socket create error");

    // 除 sin_family 参数外，sockaddr_in 内容以网络字节顺序表示。
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    // bind 函数返回 0 表示绑定成功，-1 表示绑定失败。
    int rebid = bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr));
    errif(rebid == -1, "socket bind error");

    // listen 函数返回 0 表示调用成功，-1 表示调用失败。
    int reltn = listen(sockfd, SOMAXCONN);
    errif(reltn == -1, "sockfd listen error");

    struct sockaddr_in clnt_addr;
    bzero(&clnt_addr, sizeof(clnt_addr));
    // accept 函数要求第三个参数是 socklen_t 类型。
    socklen_t clnt_addr_len = sizeof(clnt_addr);

    // accept 函数接受一个客户端请求后，若连接成功会返回一个新的非负 SOCKFD 值，若连接失败返回 -1。
    int clnt_sockfd = accept(sockfd, (sockaddr*)&clnt_addr, &clnt_addr_len);
    errif(clnt_sockfd == -1, "socket accept error");

    printf("new client fd %d! IP: %s Port: %d\n", clnt_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

    return 0;
}
