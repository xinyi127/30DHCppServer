#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "src/util.h"

#define BUFFER_SIZE 1024

int main(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    // 实际上 sin_addr 只有 s_addr 一个成员变量，之所以要设计成结构体是因为这样可以保证可移植性和可扩展性
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(1234);

    errif(connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket connect error");

    while(true){
        char buf[BUFFER_SIZE];
        memset(buf, 0, sizeof(buf));
        scanf("%s", buf);
        ssize_t write_bytes = write(sockfd, buf, sizeof(buf));
        if(write_bytes == -1) { // 写数据时出错
            printf("socket already disconnected, can't write any more!");
            break;
        }

        memset(buf, 0, sizeof(buf));
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
        if(read_bytes > 0){
            printf("message from server: %s\n", buf);
        } else if(read_bytes == 0){ // 读取到了文件末尾，或者socket连接被关闭
            printf("server socket disconnected!\n");
            break;
        } else if(read_bytes == -1){ // 出错
            close(sockfd);
            errif(-1, "socket read error");
        }
    }
    close(sockfd);
    return 0;
}
