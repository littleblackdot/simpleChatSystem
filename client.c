#include "dataParse.h"
#include "clientWork.h"

void showMeun();
void sigHander(int sig);

int main(){
    int sockid = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    char buffer[1024];
    int option;
    addr.sin_family = AF_INET;//使用IPV4 TCP/IP协议的ip地址
    addr.sin_port = htons(11234);//转换字节序
    addr.sin_addr.s_addr = inet_addr("192.168.124.131");
    signal(SIGINT, sigHander);
    clientWorkFuncType workFuncs[6] = {work_register, work_login, work_chat, work_sendFile, work_showOnline, work_superOperate};
    if(connect(sockid, (struct sockaddr *)&addr, sizeof(addr)) != 0){
        perror("connect failed");
        exit(1);
    }
    printf("已进入聊天室........\n");
    while(1){
        showMeun();
        printf("请选择：\n");
        scanf("%d", &option);
        workFuncs[option-1](sockid);
    } 
    shutdown(sockid, SHUT_RDWR);
    wait(NULL);
    
    return 0;
}

void sigHander(int sig){
    if(sig == SIGINT){
        wait(NULL);
        exit(0);
    }
}

void showMeun(){
    printf("可用选项：\n");
    printf("1.注册\n");
    printf("2.登录\n");
    printf("3.聊天\n");
    printf("4.发送文件\n");
    printf("5.查看当前在线用户列表\n");
    printf("6.管理员操作\n");
}