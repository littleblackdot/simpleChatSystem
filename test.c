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
#include<sqlite3.h>

#include "dataParse.h"

int getTotalUserNum(int fd);

void myItoA(const int, char *);

int main(){
    char sql[1024];
    int a = 312145;
    memset(sql, 0, sizeof(sql));
    myItoA(a, sql);
    printf("%s\n", sql);
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

void myItoA(const int a, char *string){
    int temp = a;
    int i = 0;
    int j=0;
    while(temp != 0){
        string[i] = temp%10 + '0';
        temp /=10;
        i++;
    }
    i--;
    for(; j < i; j++,i--){
        string[j] = string[i] + string[j];
        string[i] = string[j] - string[i];
        string[j] = string[j] - string[i];
    }
}