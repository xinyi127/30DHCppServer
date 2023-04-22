#include <sys/socket.h>
#include <arpa/inet.h>  // 该头文件包含另一个头文件<netinet/in.h>。
#include <cstring>

int main() {
    // sockfd是socket套接字，对外提供网络通信接口。也就是文件描述符，UNIX/Linux 中的一切都是文件，甚至网络连接也是一个文件。
    // 第一个参数是IP地址类型，现在表示使用IPv4，AF_INET6表示使用IPv6。
    // 第二个参数是数据传输方式，SOCK_STREAM多用于TCP，SOCK_DGRAM多用于UDP。
    // 第三个参数是协议，0表示根据前面的两个参数自动推导协议类型。
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    // sockaddr_in存网络通信地址，在头文件<netinet/in.h>中定义，把port和addr 分开储存在两个变量中。
    struct sockaddr_in serv_addr;
    // bzero函数将对应地址的前n个字节清零，在头文件<cstring>或<string.h>中定义，因为此处是写C++，所以用前者。
    bzero(&serv_addr, sizeof(serv_addr));
    // 指代协议族，在socket编程中只能是AF_INET。
    serv_addr.sin_family = AF_INET;
    // 存储IP地址，使用in_addr这个数据结构，实际上在in_addr中只有s_addr这一个成员。
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // 存储端口号。htons负责将主机字节顺序转换为网络字节顺序，host to net，s表示针对两个字节（16位），还有l，表示针对四个字节（32位）。
    serv_addr.sin_port = htons(8888);
    // P.s. socket_in除以上三者之外还有第四个成员，只是为了让sockaddr与sockaddr_in两个数据结构保持大小相同而保留的空字节，不使用。
    
    // 将socket地址与文件描述符sockfd绑定。定义时使用专用socket地址；为了便于绑定时转为通用socket。
    // 专用socket地址表示特定的通信协议、IP地址和端口号，而通用socket地址可以接收任何类型（AF_INET、AF_UNIX等）的socket地址。
    bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr));



    return 0;
}
