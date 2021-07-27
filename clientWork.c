#include "clientWork.h"


void work_register(int sockid ){
    msgToServer msg_s;
    msgToClient msg_c;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    memset(msg_s.userInfo.name, 0, sizeof(msg_s.userInfo.name));
    memset(msg_s.userInfo.pwd, 0, sizeof(msg_s.userInfo.pwd));
    printf("输入用户名：\n");
    scanf("%s", msg_s.userInfo.name);
    printf("输入密码：\n");
    scanf("%s", msg_s.userInfo.pwd);
    msg_s.action = Register;
    formatMsgToJson_msgToServer(msg_s, buffer);
    write(sockid, buffer, strlen(buffer));
    memset(buffer, 0, sizeof(buffer));

    recv(sockid, buffer,  sizeof(buffer), 0);
    parseJsonData_Client(&msg_c, buffer);
    if(msg_c.result == 1){
        printf("注册成功，你的帐号id是：%d\n", atoi(msg_c.message));
    }else{
        printf("注册失败， 错误码：%d,请联系管理员处理\n");
    }
    
}

void work_login(int sockid){
    msgToServer msg_s;
    msgToClient msg_c;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    memset(msg_s.userInfo.name, 0, sizeof(msg_s.userInfo.name));
    memset(msg_s.userInfo.pwd, 0, sizeof(msg_s.userInfo.pwd));
    printf("输入帐号id：\n");
    scanf("%d", &msg_s.userInfo.id);
    printf("输入密码：\n");
    scanf("%s", msg_s.userInfo.pwd);
    msg_s.action = Login;
    formatMsgToJson_msgToServer(msg_s, buffer);
    write(sockid, buffer, strlen(buffer));
    
    memset(buffer, 0, sizeof(buffer));
    recv(sockid, buffer,  sizeof(buffer), 0);
    parseJsonData_Client(&msg_c, buffer);
    //printf("recv: %s", buffer);
    if(msg_c.result == 1){
        printf("登录成功：\n");
    }else{
        switch(msg_c.reasonCode){
            case 1: printf("登录失败，密码错误\n"); break;
            case 0: printf("登录失败， 帐号不存在，请先注册\n");break;
            default : break;
        }
    }
}


void work_chat(int sockid){

}

void work_sendFile(int sockid){

}

void work_showOnline(int sockid){
    msgToServer msg_s;
    msgToClient msg_c;
    char buffer[BUFFER_SIZE];

    msg_s.action = showOnline;
    memset(buffer, 0, sizeof(buffer));
    formatMsgToJson_msgToServer(msg_s, buffer);
    write(sockid, buffer, strlen(buffer));

    memset(buffer, 0, sizeof(buffer));
    recv(sockid, buffer,  sizeof(buffer), 0);
    parseJsonData_Client(&msg_c, buffer);
    printf("在线用户");
    for(int i = 0; i <msg_c.nameNum; i++){
        printf("%s\n", msg_c.names[i]);
    }
}

void work_superOperate(int sockid){

}