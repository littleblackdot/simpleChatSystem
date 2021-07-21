#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include<sys/types.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/wait.h>

#include "dataParse.h"

int main(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int sockfd2;
    struct sockaddr_in addr;
    char buffer[1024];
    addr.sin_family = AF_INET;//使用IPV4 TCP/IP协议的ip地址
    addr.sin_port = htons(11234);//转换字节序
    addr.sin_addr.s_addr = inet_addr("192.168.124.131");
    if(connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) != 0){
        perror("connect failed");
        exit(1);
    }
    int pid = fork();
    if(pid < 0) {
        perror("fork failed");
        exit(1);
    }
    if(pid > 0){
        while(1){
            msgToServer msg;
            msg.action = Register;
            msg.userInfo.id = 1234;
            memset(msg.userInfo.name, 0, sizeof(msg.userInfo.name));
            memset(msg.userInfo.pwd, 0, sizeof(msg.userInfo.pwd));
            memset(msg.message, 0, sizeof(msg.message));
            memset(buffer, 0, sizeof(buffer));
            strncpy(msg.userInfo.name, "root", 4);
            strncpy(msg.userInfo.pwd, "sdasw", 4);
            strncpy(msg.message, "hello ", 5);
            formatMsgToJson_msgToServer(msg, buffer);
            write(sockfd, buffer, sizeof(buffer));
            if(strncmp(buffer, "end", 3) == 0 ){
                kill(pid, SIGINT);
                exit(0);
            }
            sleep(4);
        }  
    }else if(pid == 0){
        while(1){
            memset(buffer, 0, sizeof(buffer)); 
            read(sockfd, buffer, sizeof(buffer));
            if(strncmp(buffer, "end", 3) == 0 || strlen(buffer) == 0){
                kill(getppid(), SIGINT);
                exit(0);
            }
            printf("receive from server: %s\n", buffer);
        }    
    }  
    shutdown(sockfd, SHUT_RDWR);
    wait(NULL);
    return 0;
}