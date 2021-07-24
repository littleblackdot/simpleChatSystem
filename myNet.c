#include "myNet.h"


int socketInit_TCP(int port, const char* addr){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in hostNetInfo;
    socklen_t addrLen = sizeof(struct sockaddr_in);
    hostNetInfo.sin_family = AF_INET;//使用IPV4 TCP/IP协议的ip地址
    hostNetInfo.sin_port = htons(port);//转换字节序
    hostNetInfo.sin_addr.s_addr = inet_addr(addr);
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if(bind(sockfd, (struct sockaddr *)&hostNetInfo, addrLen) != 0){
        perror("bind failed");
        exit(1);
    }
    if(listen(sockfd, SERVER_LISTEN_CAPACITY) != 0){
        perror("listen failed");
        exit(1);
    }
    return sockfd;
}