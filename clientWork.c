#include "clientWork.h"



void work_register(int sockid ){
    if(isLogin ==1){
        printf("请退出登录后再注册\n");
        return;
    }
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
    if(isLogin == 1){
        printf("已登录，请勿重复操作");
    }
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
        isLogin = 1;
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
    if(isLogin != 1){
        printf("请先登录再操作\n");
    }
    int option;
    printf("请选择：\n1.私聊\n2.群发消息\n");
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
    if(isLogin != 1){
        printf("请先登录再操作\n");
        return;
    }
    msgToServer msg_s;
    char buffer[BUFFER_SIZE];
    int option;
    printf("请选择：\n");
    printf("1.禁言\n2.解禁\n3.踢人\n");
    scanf("%d", &option);
    printf("请输入操作对象\n");
    memset(msg_s.userInfo.name, 0, sizeof(msg_s.userInfo.name));
    scanf("%s", msg_s.userInfo.name);
    msg_s.actionOption = option-1;
    msg_s.action = superOperation;
    memset(buffer, 0, sizeof(buffer));
    formatMsgToJson_msgToServer(msg_s, buffer);
    write(sockid, buffer, strlen(buffer));
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
    parseJsonData_Client(&msg_c, buffer);//会给msg_s.names申请内存，需要手动释放;
    //printf("recv: %s", buffer);
    if(msg_c.result == 0){
        switch(msg_c.action){
            case chatResultReturn:{
                if(msg_c.reasonCode == 0){
                    printf("信息发送失败，目标用户不在线\n");
                }else if(msg_c.reasonCode == 1){
                     printf("信息发送失败，你已被禁言\n");
                }else if(msg_c.reasonCode == 2){
                    printf("请不要对自己发信息\n");
                }
                break;
            }
            case sendFileRequest:{
                break;
            }
            case superOperationResultReturn:{
                if(msg_c.reasonCode == 0){
                    printf("管理员操作，权限不足,充会员说不定可以解决......\n");
                }else if(msg_c.reasonCode ==1){
                    printf("操作失败，目标用户不在线\n");
                }
                break;
            } 
            default : break;
        }
        if(msg_c.nameNum !=0){
            free(msg_c.names);
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
        case sendFileRequest:{
            
            break;
        }
        case superOperationResultReturn:{
            switch(msg_c.reasonCode){
                case 0: printf("你已被禁言......\n"); break;
                case 1: printf("禁言状态取消\n"); break;
                case 2: {
                    printf("你已被管理员强制下线\n");
                    kill(getppid(), SIGALRM);
                    shutdown(sockid, SHUT_RDWR);
                    exit(0);
                    break;
                }
                case 3: printf("用户：%s已被禁言\n", msg_c.message);  break;
                case 4: printf("用户：%s已被解禁\n", msg_c.message);  break;
                case 5: printf("用户：%s已被强制下线\n", msg_c.message);  break;
                default : break;
            }
            
            break;
        }
        default : break;
    }
    if(msg_c.nameNum !=0){
        free(msg_c.names);
    }
}