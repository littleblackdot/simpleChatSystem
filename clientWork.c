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
        return;
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
        return;
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
    msgToServer msg_s;
    FileInfo fileInfo;
    struct stat filestat;
    char buffer[BUFFER_SIZE];
    char filePath[FILEPATHLEN];
    struct msgbuf msg;
    
    printf("输入文件路径：\n");
    bzero(filePath, sizeof(filePath));
    scanf("%s", filePath);
    int fd = open(filePath, O_RDONLY);
	if(fd == -1){
		perror("open file error");
		return ;
	}
    //从路径中截取文件名
	char *p = strrchr(filePath, '/') + 1;
	bzero(fileInfo.name, sizeof(fileInfo.name));
	fstat(fd ,&filestat);
    strncpy(fileInfo.name, p, strlen(p));
    fileInfo.size = filestat.st_size;
    fileInfo.count = 8;
    fileInfo.blockSize = FILEDIVLEN;
    bzero(msg_s.message, sizeof(msg_s.message));
    formatFileInfoToJson(fileInfo, msg_s.message);

    printf("输入文件发送对象:\n");
    bzero(msg_s.userInfo.name, sizeof(msg_s.userInfo.name));
    scanf("%s", msg_s.userInfo.name);

    memset(buffer, 0, sizeof(buffer));
    bzero(msg.mtext, sizeof(msg.mtext));
    strncpy(msg.mtext, filePath, strlen(filePath));

    msg.mtype = 1;
    if(msgsnd(msgid, &msg, strlen(msg.mtext), IPC_NOWAIT) == -1) {
        perror("msg send error");
        return;
    }
    msg_s.action = fileOperation;
    msg_s.actionOption = 0;// 0, 请求文件发送， 1, 接受文件, 2，拒绝接受文件
    formatMsgToJson_msgToServer(msg_s, buffer);
    write(sockid, buffer, strlen(buffer));
    close(fd);
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
    if(msg_c.result == 0){//封装......
        switch(msg_c.action){
            case chatResultReturn:{
                switch(msg_c.reasonCode){
                    case 0: printf("信息发送失败，目标用户不在线\n");break;
                    case 1: printf("信息发送失败，你已被禁言\n"); break;
                    case 2: printf("请不要对自己发信息\n"); break;
                    default : break;
                }
                break;
            }
            case fileOperationResultReturn:{
                switch(msg_c.reasonCode){
                    case 0: printf("发送失败，对方不在线\n"); break;
                    case 1: {
                        struct msgbuf msg;
                        printf("对方拒绝了你的文件传送请求\n");
                        msgrcv(msgid, &msg, sizeof(msg.mtext), 1, 0);
                        break;
                    }
                    case 2: printf("文件发送用户下线，文件传输取消\n"); break;
                    default: break;
                }
                break;
            }
            case superOperationResultReturn:{
                switch(msg_c.reasonCode){
                    case 0:  printf("管理员操作，权限不足,充会员说不定可以解决......\n");break;
                    case 1: printf("操作失败，目标用户不在线\n"); break;
                    default : break; 
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
            struct msgbuf msg;
            bzero(&msg, sizeof(msg));
            strncpy(msg.mtext, buffer, strlen(buffer));
            msg.mtype = 2;
            msgsnd(msgid, &msg, strlen(msg.mtext), IPC_NOWAIT);
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
    if(msg_c.nameNum != 0){
        free(msg_c.names);
    }
}

void fileSendHelper(int sockid, const msgToClient *pmsg, const char *filepath){
    int option ;
    msgToServer msg_s;
    char buffer[BUFFER_SIZE];
    if(pmsg->reasonCode == 0){
        printf("你收到了一个来自用户：%s的文件传输请求，请选择\n", pmsg->names[0]);
        printf("1.同意\n2.拒绝\n");
        scanf("%d", &option);
        bzero(msg_s.userInfo.name, sizeof(msg_s.userInfo.name));
        strncpy(msg_s.userInfo.name, pmsg->names[0], strlen(pmsg->names[0]));
        msg_s.action = fileOperation;
        if(option == 1){
            printf("文件接收开始\n");
            msg_s.actionOption = 2;
            bzero(buffer, sizeof(buffer));
            formatMsgToJson_msgToServer(msg_s, buffer);
            send(sockid, buffer, strlen(buffer), 0);
            //开启tcp监听，接受udp数据.......
            FileInfo fileInfo;
            parseJsonData_FileInfo(&fileInfo, pmsg->message);
            receiveFile(sockid, &fileInfo);
            return; 
        }
        msg_s.actionOption = 1;
        bzero(buffer, sizeof(buffer));
        formatMsgToJson_msgToServer(msg_s, buffer);
        send(sockid, buffer, strlen(buffer), 0);
        return;
    }
    printf("对方接受了你的文件传送请求，文件传输开始.......\n");
    sendFile(pmsg->message, filepath);
}

void* checkFilesendRequest(void* psockid){
    struct msgbuf msg;
    int sockid = *(int*)psockid;
    bzero(&msg, sizeof(msg));
    if(msgrcv(msgid, &msg, sizeof(msg.mtext), 2, IPC_NOWAIT) > 0) {
        char filePath[FILEPATHLEN];
        msgToClient msg_c;
        parseJsonData_Client(&msg_c, msg.mtext);
        //printf("msgToClient:%s\n", msg.mtext);
        bzero(&msg, sizeof(struct msgbuf));
        msgrcv(msgid, &msg, sizeof(msg.mtext), 1, IPC_NOWAIT);
        bzero(filePath, sizeof(filePath));
        strncpy(filePath, msg.mtext, strlen(msg.mtext));
        fileSendHelper(sockid, &msg_c, filePath);
        bzero(&msg, sizeof(struct msgbuf));
    }
}

void receiveFile(int sockid, FileInfo *pfileInfo){
    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in hostAddr, peerAddr;
    socklen_t addrLen = sizeof(struct sockaddr_in);
    int opt = 1;
    char buffer[BUFFER_SIZE];
    bzero(&hostAddr, sizeof(hostAddr));
    
    getsockname(sockid, (struct sockaddr*)&hostAddr, (socklen_t*)&addrLen);
    int fd  = open(pfileInfo->name, O_RDWR|O_CREAT|O_TRUNC|O_APPEND, 0655);
    if(fd < 0){
        perror("file create fail");
    }

    setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); 
    if(bind(listenFd, (struct sockaddr *)&hostAddr, sizeof(hostAddr)) != 0){
        perror("listenfd bind failed");
    }

    if(listen(listenFd, 9) != 0){
        perror(" listen failed");
    }
    for(int i = 0; i < 8; i++){
        bzero(&peerAddr, sizeof(peerAddr));
        int connfd = accept(listenFd, (struct sockaddr*)&peerAddr, &addrLen);
        printf("accept : %d\n", i);
        if(connfd == -1){
            perror("accept error");
        }
        int pid = fork();
        if(pid < 0){
            perror("fork error");
            exit(1);
        }
        if(pid == 0){
            //子进程接收文件分块
            int udpfd = socket(AF_INET, SOCK_DGRAM, 0);
            int count = pfileInfo->blockSize;
            PartOfFile part;
            getsockname(connfd, (struct sockaddr *)&hostAddr, &addrLen);
            setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); 
            bind(udpfd, (struct sockaddr *)&hostAddr, sizeof(hostAddr));
           
            while(count == pfileInfo->blockSize){
                bzero(buffer, sizeof(buffer));
                recvfrom(udpfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&peerAddr, (socklen_t *)&addrLen);
                //printf("receive:%s\n", buffer);
                bzero(part.body, sizeof(part.body));
                parseJsonData_PartOfFile(&part, buffer);
                count = part.realsize;
                pthread_mutex_lock(&mutex_fd);
                lseek(fd, part.offset, SEEK_SET);
                printf("%d:part.body:%s\n", i, part.body);
                write(fd, part.body, part.realsize);
                pthread_mutex_unlock(&mutex_fd);
            }
            exit(1);
        }   
    }
    wait(NULL);
}


void sendFile(const char* goalAddr, const char* filePath){
    int fd = open(filePath, O_RDONLY);
    int opt = 1;
    struct sockaddr_in addr, addr2;
    struct stat filestat;
    socklen_t addrLen = sizeof(struct sockaddr_in);
    char buffer[BUFFER_SIZE];

    if(fd < 0){
        perror("open file error");
        return ;
    }

    bzero(&addr, sizeof(addr));
    parseIPADDR(goalAddr, &addr);
    addr.sin_family = AF_INET; 
    fstat(fd ,&filestat);
    int connfd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(connfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if(connect(connfd, (struct sockaddr *)&addr, sizeof(addr)) != 0){
        perror("connect failed");
        exit(1);
    }else{
        printf("Connect \n");
    }

    for(int i = 0; i < 8; i++){
        int pid = fork();
        if(pid == 0){
            int connfd = socket(AF_INET, SOCK_STREAM, 0);
            PartOfFile part;
            int partFileSize = 0;
            int blockSize = FILEDIVLEN;
            int udpfd;
            if(connect(connfd, (struct sockaddr *)&addr, sizeof(addr)) != 0){
                perror("connect failed");
                exit(1);
            }
            udpfd = socket(AF_INET, SOCK_DGRAM, 0);
            bzero(&addr2, sizeof(addr2));
            getpeername(connfd, (struct sockaddr *)&addr2, &addrLen);
           // printf("acceptAddr3:%s : %d\n", inet_ntoa(addr2.sin_addr), addr2.sin_port);
            part.offset = i * (filestat.st_size/8);
            part.seq = i;
            partFileSize = i < 7 ? (filestat.st_size/8) : filestat.st_size - (filestat.st_size/8)*i;
            while(partFileSize > 0){
                lseek(fd, part.offset, SEEK_SET);
                bzero(part.body, sizeof(part.body));
                bzero(buffer, sizeof(buffer));
                part.realsize = partFileSize > blockSize ? blockSize: partFileSize;
                partFileSize -= part.realsize;
                read(fd, part.body, part.realsize);
                formatPartOfFileInfoToJson(part, buffer);
                printf("%d send:%s\n", i, buffer);
                sendto(udpfd ,buffer, strlen(buffer), 0, (struct sockaddr *)&addr2, sizeof(addr2));
                part.offset += partFileSize;
            }
            exit(1);
        }
        wait(NULL);
    }
}

void parseIPADDR(const char* addrString, struct sockaddr_in *addr){
    char temp[IPV4ADDRLEN];
    int i = 0;
    bzero(temp, sizeof(temp));
    while(addrString[i] != ':'){
        temp[i] = addrString[i];
        i++;
    }
    i++;
    addr->sin_addr.s_addr = inet_addr(temp);
    addr->sin_port = atoi(addrString+i);
}