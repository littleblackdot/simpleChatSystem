#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include<sys/types.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/wait.h>

#include "dataParse.h"

void sigHander(int sign);

int main(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int sockfd2;
    struct sockaddr_in addr;
    struct sockaddr_in clientAddr={0};
    int clAddrLen = sizeof(struct sockaddr);;
    char buffer[1024];
    addr.sin_family = AF_INET;//使用IPV4 TCP/IP协议的ip地址
    addr.sin_port = htons(11234);//转换字节序
    addr.sin_addr.s_addr = inet_addr("192.168.124.131");
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if(bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) != 0){
        perror("bind failed");
    }
    if(listen(sockfd, 5) != 0){
        perror("listen failed");
        exit(1);
    }
    
    while(1){
        sockfd2 = accept(sockfd, (struct sockaddr *)&clientAddr, &clAddrLen);
        printf("accept client connection\n");
        int pid = fork();
        if(pid < 0){
            perror("fork failed");
            exit(1);
        }
        if(pid == 0){
            int ppid = fork();
            if(ppid < 0){
                perror("fork failed");
                exit(1);
            }
            if(ppid == 0){
                while(1){
                    msgToClient msg;
                    msg.names = (char(*)[40])malloc(40*4);
                    msg.result = 1;
                    msg.nameNum = 4;
                    msg.action = resultReturn;
                    memset(msg.names[0], 0, 40);
                    memset(msg.names[1], 0, 40);
                    memset(msg.names[2], 0, 40);
                    memset(msg.names[3], 0, 40);
                    memset(msg.message, 0, sizeof(msg.message));
                    strncpy(msg.names[0],"zhang", 5);
                    strncpy(msg.names[1],"little", 6);
                    strncpy(msg.names[2],"jianxai", 7);
                    strncpy(msg.names[3],"zxxx", 4);
                    strncpy(msg.message, "check out", 9);
                    memset(buffer, 0, sizeof(buffer));
                    formatMsgToJson_msgToClient(msg, buffer);
                    write(sockfd2, buffer, sizeof(buffer));
                    if(strncmp(buffer, "end", 3) == 0 || strlen(buffer) == 0){
                        shutdown(sockfd2, SHUT_RDWR);
                        kill(getppid(), SIGINT);
                        exit(0);
                    }
                    sleep(3);
                }  
            }else if(ppid > 0){
                signal(SIGINT, sigHander);
                while(1){
                    msgToServer msg;
                    memset(buffer, 0, sizeof(buffer));
                    read(sockfd2, buffer, sizeof(buffer));
                    parseJsonData_Server(&msg, buffer);
                    printf("get the message : \n");
                    showMsg_msgToServer(msg);
                    if(strncmp(buffer, "end", 3) == 0 || strlen(buffer) == 0){
                        kill(ppid, SIGINT);
                        wait(NULL);
                        shutdown(sockfd2, SHUT_RDWR);
                        exit(0);
                    }
                }   
            }    
        }    
    }
    shutdown(sockfd2, SHUT_RDWR);
    wait(NULL);
    return 0;
}

void sigHander(int sig){
    if(sig == SIGINT){
        wait(NULL);
        exit(1);
    }
}