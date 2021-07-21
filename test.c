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

int main(){
    int fd = open("userInfo.txt", O_RDWR|O_CREAT, 0655);
    char buffer1[20] = "total user: 1\n";
    char buffer2[20] = "total user: 2\n";
    lseek(fd, 0, SEEK_SET);
    write(fd, buffer1, strlen(buffer1));
    sleep(5);
    lseek(fd, 0, SEEK_SET);
    write(fd, buffer2, strlen(buffer2));
    lseek(fd, 0, SEEK_END);
    write(fd, buffer1, strlen(buffer1));
    close(fd);
    return 0;
}