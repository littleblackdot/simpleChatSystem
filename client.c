#include "dataParse.h"
#include "clientWork.h"

int main(){
    int sockid = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    char buffer[1024];
    int option;
    addr.sin_family = AF_INET;//使用IPV4 TCP/IP协议的ip地址
    addr.sin_port = htons(11234);//转换字节序
    addr.sin_addr.s_addr = inet_addr("192.168.124.131");
    if(connect(sockid, (struct sockaddr *)&addr, sizeof(addr)) != 0){
        perror("connect failed");
        exit(1);
    }
        printf("已进入聊天室........\n");
        printf("请选择：\n");
        printf("1.登陆\n");
        printf("2.注册\n");
        printf("3.私聊\n");
        printf("4.群聊\n");
        scanf("%d", &option);
        if(option == 1){
            work_register(sockid);
        }
    
    shutdown(sockid, SHUT_RDWR);
    wait(NULL);
    return 0;
}