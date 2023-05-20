#include "util.cpp" // errif
#include <sys/socket.h> // socket 及相关函数和数据结构
#include <arpa/inet.h> // IP 地址转换相关的函数和宏，该头文件引入了 <netinet/in.h>

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");
    // sockaddr_in 专用于 IPv4，sockaddr_in6 专用于 IPv6，sockaddr 通用地址结构
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    // 将点分十进制IP地址转换为网络字节序无符号长整型IP地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htos(8888); // 主机字节序到网络字节序
    // 将一个未连接的socket与特定的 IP 地址和端口号相关联。第二个参数本质上是一种"将派生类指针转换为基类指针"的操作
    errif(bind(sockfd, （sockaddr*)&serv_addr, sizeof(serv_addr) == -1, "socket bind error");
    // 监听 sockfd 绑定的对应 IP 地址和端口
    errif(listen(socket, SOMAXCONN) == -1, "socket listen error");


}
