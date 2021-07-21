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
                    memset(buffer, 0, sizeof(buffer));
                    scanf("%s", buffer);
                    write(sockfd2, buffer, sizeof(buffer));
                    if(strncmp(buffer, "end", 3) == 0 || strlen(buffer) == 0){
                        kill(getppid(), SIGINT);
                        exit(0);
                    }
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
                  /*   printf("%c %d | %c %d | %c %d | %c %d | %c %d |\n", msg.name[0], msg.name[0], msg.name[1], msg.name[1],
            msg.name[2] ,msg.name[2], msg.name[3], msg.name[3], msg.name[4], msg.name[4]); */
                    if(strncmp(buffer, "end", 3) == 0 || strlen(buffer) == 0){
                        kill(pid, SIGINT);
                        wait(NULL);
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