#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include<sys/types.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<arpa/inet.h>
#include<sys/wait.h>

#include "dataParse.h"

int getTotalUserNum(int fd);


int main(){
    int fd = open("userInfo.txt", O_RDWR|O_CREAT, 0655);
    char buffer1[30] = "total user: 112312\n";
    char buffer2[30] = "total user: 2839021\n";
    lseek(fd, 0, SEEK_SET);
    write(fd, buffer1, strlen(buffer1));
    lseek(fd, 0, SEEK_SET);
    write(fd, buffer2, strlen(buffer2));
    lseek(fd, 0, SEEK_END);
    write(fd, buffer1, strlen(buffer1));
    printf("%d \n", getTotalUserNum(fd));
    close(fd);
    return 0;
}

int getTotalUserNum(int fd){
    int ret = 0;
    int index = 12;
    char buffer[100];
    memset(buffer, 0, sizeof(buffer));
    lseek(fd, 0, SEEK_SET);
    read(fd, buffer, sizeof(buffer));
    while(buffer[index] != '\n'){
        ret = ret * 10 + buffer[index]-'0';
	index++;
    }
    return ret;
}

