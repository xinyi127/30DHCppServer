#include "util.h" // 自定义的错误处理函数 util
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h> // read 和 write 函数

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

    // 建立一个socket连接后，使用 <unistd.h> 头文件中 read 和 write 函数来进行网络接口的数据读写操作。
    // 这两个函数用于 TCP 连接。如果是 UDP，需要使用 sendto 和 recvfrom。
    while (true) {
        // 定义缓冲区，存放从客户端读到的数据。
        char buf[1024];
        // 清空缓冲区。
        bzero(&buf, sizeof(buf));
        // 使用 write() 可以向套接字中写入数据，使用 read() 可以从套接字中读取数据。
        // size_t 是通过 typedef 声明的 unsigned int 类型；ssize_t 在 "size_t" 前面加了一个"s"，代表 signed，即 ssize_t 是通过 typedef 声明的 signed int 类型。
        // read() 的原型：ssize_t read(int fd, void *buf, size_t nbytes);
        // fd 为要读取的文件的描述符，buf 为要接收数据的缓冲区地址，nbytes 为要读取的数据的字节数。
        ssize_t read_bytes = read(clnt_sockfd, buf, sizeof(buf));
        // read() 函数会从 fd 文件中读取 nbytes 个字节并保存到缓冲区 buf，成功则返回读取到的字节数。
        if(read_bytes > 0){
            printf("message from client fd %d: %s\n", clnt_sockfd, buf);
            // 将相同数据写回客户端。
            // write() 的原型：ssize_t write(int fd, const void *buf, size_t nbytes);
            // fd：要写入的文件的描述符。buf：要写入的数据的缓冲区地址。nbytes：要写入的数据的字节数。
            write(clnt_sockfd, buf, sizeof(buf));
        } else if(read_bytes == 0){ // read() 返回 0 表示 EOF ，规定如此。通常是对端关闭了连接。
            printf("client fd %d disconnect\n", clnt_sockfd);
            // 小细节：Linux 系统的文件描述符理论上是有限的，在使用完一个 fd 之后，需要使用头文件 <unistd.h> 中的 close 函数关闭。
            close(clnt_sockfd);
            break;
        } else if(read_bytes == -1){ // read() 返回 -1，表示发生错误。
            close(clnt_sockfd);
            errif(true, "socket read error");
        }
    }
    // 使用完一个 fd 后应该及时关闭。
    close(sockfd);
    return 0;
}
