#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>

int main() {
    // 创建 socket 文件描述符
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // 专用 socket 地址
    struct sockaddr_in serv_addr;
    // 清空
    bzero(&serv_addr, sizeof(serv_addr));
    // 设置地址族，IP 地址和端口
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);
    
    // 与 server.cpp 监听的端口建立连接。注意将专用 socket 地址转为通用 socket 地址。
    // connect 函数建立与指定套接字的连接。如果未发生错误，连接将返回零，否则返回 SOCKET_ERROR。
    connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr));

    return 0;
}


// 客户端:
// socket()-->connect()-->read()/write()-->close()
// socket()//创建套接字
// connect()//请求连接
// read()/write()//进行数据交换
// close()//断开连接

