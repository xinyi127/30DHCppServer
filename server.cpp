#include <sys/socket.h>
#include <arpa/inet.h>  // 该头文件包含另一个头文件 <netinet/in.h>。
#include <string.h>
#include <stdio.h>

int main() {
    // sockfd 是 socket 套接字，对外提供网络通信接口。也就是文件描述符，UNIX/Linux 中的一切都是文件，甚至网络连接也是一个文件。
    // 第一个参数是 IP 地址类型，现在表示使用 IPv4，AF_INET6 表示使用 IPv6 。
    // 第二个参数是数据传输方式，SOCK_STREAM 多用于 TCP，SOCK_DGRAM 多用于 UDP。
    // 第三个参数是协议，0 表示根据前面的两个参数自动推导协议类型。
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // sockaddr_in 存网络通信地址，在头文件 <netinet/in.h> 中定义，把 port 和 addr 分开储存在两个变量中。
    struct sockaddr_in serv_addr;
    // bzero 函数将对应地址的前 n 个字节清零，在头文件 <cstring> 或 <string.h> 中定义，因为此处是写 C ，所以用后者。
    bzero(&serv_addr, sizeof(serv_addr));
    // 指代协议族，在 socket 编程中只能是 AF_INET 。
    serv_addr.sin_family = AF_INET;
    // 存储 IP 地址，使用 in_addr 这个数据结构，实际上在 in_addr 中只有 s_addr 这一个成员。
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // 存储端口号。htons 负责将主机字节顺序转换为网络字节顺序，host to net，s 表示针对两个字节（16位），还有 l，表示针对四个字节（32位）。
    serv_addr.sin_port = htons(8888);
    // P.s. socket_in 除以上三者之外还有第四个成员，只是为了让 sockaddr 与 sockaddr_in 两个数据结构保持大小相同而保留的空字节，不使用。

    // 将 socket 地址与文件描述符 sockfd 绑定。定义时使用专用 socket 地址；为了便于绑定时转为通用 socket。
    // 专用 socket 地址表示特定的通信协议、IP 地址和端口号，而通用 socket 地址可以接收任何类型（AF_INET、AF_UNIX等）的 socket 地址。
    bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr));

    // 使用 listen 函数监听这个 socket 端口 sockfd。listen 函数使用主动连接套接字变为被连接套接口，使得一个进程可以接受其它进程的请求，从而成为一个服务器进程。
    // SOMAXCONN 定义了系统中每一个端口最大的监听队列的长度，这是个全局的参数，默认值为 128。
    listen(sockfd, SOMAXCONN);

    struct sockaddr_in clnt_addr;
    // socklen_t 是一种数据类型，在 32 位机下，size_t 和 int 的长度相同，都是32 bits。socket 编程中的 accept 函数的第三个参数的长度必须和 int 的长度相同。
    socklen_t clnt_addr_len = sizeof(clnt_addr);
    bzero(&clnt_addr, sizeof(clnt_addr));
    // 要接受一个客户端连接，需要使用 accept 函数，accept 需要写入客户端 socket 长度，所以需要定义一个类型为 socklen_t 的变量，并传入这个变量的地址。
    // accept 函数会阻塞当前程序，直到有一个客户端 socket 接受后程序才会往下运行。
    int clnt_sockfd = accept(sockfd, (sockaddr*)&clnt_addr, &clnt_addr_len);
    // clnt_sockfd 是通过 accept() 接受的客户端连接的文件描述符。
    // inet_ntoa函数将 (Ipv4) Internet 网络地址转换为采用 Internet 标准点十进制格式的 ASCII 字符串。成功则 inet_ntoa 返回指向静态缓冲区的字符指针，否则返回 NULL。
    // 将一个无符号短整形数从网络字节顺序转换为主机字节顺序。网络字节顺序 NBO ：按从高到低的顺序存储，在网络上使用同一的网络字节顺序，可避免兼容性问题。
    printf("new client fd %d! IP: %s Port: %d\n", clnt_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
    return 0;
}


// 关于listen()和accept()这一段，可以想象有人从很远的地方通过一个你在侦听 (listen()) 的端口连接 (connect()) 到你的机器。它的连接将加入到等待接受 (accept()) 的队列 中。
// 调用 accept() 告诉它你有空闲的连接。它将返回一个新的套接字文件描述符，这样你就有两个套接字了。
// 原来的一个还在侦听你的那个端口， 新的在准备发送 (send()) 和接收 ( recv()) 数据。这就是这个过程！
