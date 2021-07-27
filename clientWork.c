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
        int pid = fork();
        if(pid == 0){//登录成功创建一个进程来接收来自服务段的信息；
            while(1){
                messReceive(sockid);
            }
        }
    }else{
        switch(msg_c.reasonCode){
            case 1: printf("登录失败，密码错误\n"); break;
            case 0: printf("登录失败， 帐号不存在，请先注册\n");break;
            default : break;
        }
    }
   
}


void work_chat(int sockid){
    int option;
    printf("请选择：\n1.私聊\n 2.群发消息\n");
    scanf("%d",&option);
    msgToServer msg_s;

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    memset(msg_s.userInfo.name, 0, sizeof(msg_s.userInfo.name));
    msg_s.action = Chat;
    msg_s.actionOption = option-1;//聊天动作选项：0.私聊,1.群聊
    if(option == 1){
        printf("输入私聊对象：\n");
        scanf("%s", msg_s.userInfo.name);
    }

    printf("输入消息内容：\n");
    memset(msg_s.message, 0, sizeof(msg_s.message));
    scanf("%s", msg_s.message);
    formatMsgToJson_msgToServer(msg_s, buffer);
    write(sockid, buffer, strlen(buffer));

}

void work_sendFile(int sockid){

}

void work_showOnline(int sockid){
    msgToServer msg_s;
    char buffer[BUFFER_SIZE];

    msg_s.action = showOnline;
    memset(buffer, 0, sizeof(buffer));
    formatMsgToJson_msgToServer(msg_s, buffer);
    write(sockid, buffer, strlen(buffer));
    
}

void work_superOperate(int sockid){

}

void messReceive(int sockid){
    msgToClient msg_c;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    if(recv(sockid, buffer,  sizeof(buffer), 0) <= 0){
        printf("服务器挂了.......\n");
        kill(getppid(), SIGINT);
        exit(1);
    }
    parseJsonData_Client(&msg_c, buffer);
    //printf("recv: %s", buffer);
    if(msg_c.result == 0){
        switch(msg_c.action){
            case chatResultReturn:{
                if(msg_c.reasonCode == 0){
                    printf("信息发送失败，目标用户不在线\n");
                }else if(msg_c.reasonCode == 0){
                     printf("信息发送失败，你已被禁言\n");
                }
                break;
            }
            case sendFileRequest:{
                break;
            }
            case superOperationResultReturn:{
                break;
            } 
            default : break;
        }
        return;
    }
    switch(msg_c.action){
        case messagePost:{
            printf("用户：%s发来信息：%s\n", msg_c.names[0], msg_c.message);
            break;
        }
        case showUsers:{
            printf("在线用户:\n");
            for(int i = 0; i <msg_c.nameNum; i++){
                printf("%s\n", msg_c.names[i]);
            }
            break;
        }
    }
    
}