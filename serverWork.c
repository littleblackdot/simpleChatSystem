#include "serverWork.h"

extern pthread_mutex_t dataBase_mutex;
extern pthread_mutex_t userList_mutex; 
void work_register(void * userList, void * userInfo, void * pOption, void * message){
    UserInfo_server *userInfo_s  = (UserInfo_server *)userInfo;
    List* list = (List*)userList;
    int option = *(int*)pOption;
    char *mess = (char*)message;

    msgToClient msg;
    char sql[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    char log[BUFFER_SIZE];
    char *errMsg;
    char **table;
    int columnCount;
    int rowCount;
    sqlite3 *db;

    msg.nameNum = 0;
    if(strlen(userInfo_s->userInfo_c.name) == 0 ||
        strlen(userInfo_s->userInfo_c.pwd) == 0){
         printf("userName or password parse error\n");
         return ;   
    }
    
    int ret = sqlite3_open("local.db", &db);
    perror_db(db, ret, "open DataBase error");
    sprintf(sql, "insert into User(username, password) values('%s', '%s');",
    userInfo_s->userInfo_c.name, userInfo_s->userInfo_c.pwd);

    pthread_mutex_lock(&dataBase_mutex);
    ret = sqlite3_exec(db, sql, NULL, NULL, &errMsg);
    perror_db(db, ret, "insert data base error");
    memset(sql, 0, sizeof(sql));
    sprintf(sql, "select (id) from User order by id desc limit 1");
    ret = sqlite3_get_table(db, sql, &table, &rowCount, &columnCount, &errMsg);
    perror_db(db, ret, "query  database error");
    userInfo_s->userInfo_c.id = atoi(table[1]);
    pthread_mutex_unlock(&dataBase_mutex);

    bzero(log, sizeof(log));
    sprintf(log, "用户%s注册，id：%d\n", userInfo_s->userInfo_c.name, userInfo_s->userInfo_c.id);
    printf(log);
    write(logfd, log, strlen(log));

    msg.result = 1;
    memset(msg.message, 0, sizeof(msg.message));
    myItoA(userInfo_s->userInfo_c.id, msg.message);
    memset(buffer, 0, sizeof(buffer));
    formatMsgToJson_msgToClient(msg, buffer);
    send(userInfo_s->sockid, buffer, strlen(buffer), 0);
    sqlite3_free_table(table);
    sqlite3_close(db);
    free(userInfo);
    free(pOption);
    free(message);
}


void work_login(void * userList, void * userInfo,  void * pOption, void * message){
    UserInfo_server *userInfo_s  = (UserInfo_server *)userInfo;
    List* list = (List*)userList;
    int option = *(int*)pOption;
    char *mess = (char*)message;
    msgToClient msg;
    char sql[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    char log[BUFFER_SIZE];
    char *errMsg;
    char **table;
    int columnCount;
    int rowCount;
    sqlite3 *db;

    msg.nameNum = 0;
    int ret = sqlite3_open("local.db", &db);
    perror_db(db, ret, "open DataBase error");
    sprintf(sql, "select * from User where id = %d ;", userInfo_s->userInfo_c.id);
    ret = sqlite3_get_table(db, sql, &table, &rowCount, &columnCount, &errMsg);
    perror_db(db, ret, "query  database error");
    if(rowCount == 0 || strncmp(userInfo_s->userInfo_c.pwd, table[columnCount*1 + 2], strlen(table[columnCount*1 + 2])) != 0){
        msg.result = 0;
        msg.reasonCode = rowCount ;//0，表示用户id不存在， 1表示密码错误
        memset(buffer, 0, sizeof(buffer));
        formatMsgToJson_msgToClient(msg, buffer);
        send(userInfo_s->sockid, buffer, strlen(buffer), 0);
        sqlite3_free_table(table);
        sqlite3_close(db);
        freeAll(userInfo, pOption, message);
        return ;
    }

    
    memset(userInfo_s->userInfo_c.name, 0, sizeof(userInfo_s->userInfo_c.name));
    strncpy(userInfo_s->userInfo_c.name, table[columnCount*1 + 1], strlen(table[columnCount*1 + 1]));
    userInfo_s->userType = (UserType)atoi(table[columnCount*1 + 3]);
    userInfo_s->status = normal;
    
    bzero(log, sizeof(log));
    sprintf(log, "用户%s登录，id：%d\n", userInfo_s->userInfo_c.name, userInfo_s->userInfo_c.id);
    printf("1\n");
    printf(log);
    write(logfd, log, strlen(log));

    pthread_mutex_lock(&userList_mutex);
    insertNode(list, *userInfo_s);
    pthread_mutex_unlock(&userList_mutex);
    //displayList(list);
    //printf("isroot %d\n", userInfo_s->userType == root);
    msg.result = 1;
    memset(buffer, 0, sizeof(buffer));
    formatMsgToJson_msgToClient(msg, buffer);
    send(userInfo_s->sockid, buffer, strlen(buffer), 0);
    sqlite3_free_table(table);
    sqlite3_close(db);
    freeAll(userInfo, pOption, message);
}


void work_chat(void * userList, void * userInfo, void * pOption, void * message){
    UserInfo_server *userInfo_s  = (UserInfo_server *)userInfo;
    List* list = (List*)userList;
    int option = *(int*)pOption;
    char *mess = (char*)message;
    int count = 0;
    msgToClient msg;
    char buffer[BUFFER_SIZE];
    char log[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    pthread_mutex_lock(&userList_mutex);
    Node_List clientNode;
    Node_List *temp0 = findNode(list, *userInfo_s, isEqual_itemSockID);
    if(temp0 == NULL){
        printf("用户列表异常，操作发起用户不在线\n");
        pthread_mutex_unlock(&userList_mutex);
        freeAll(userInfo, pOption, message);
        return;
    }
    userInfoCopy(&clientNode.item, temp0->item);
    pthread_mutex_unlock(&userList_mutex);

    if(clientNode.item.status == postBanend){
        msg.result = 0;
        msg.reasonCode = 1; //聊天错误码：0， 用户不在线， 1，被禁言
        msg.nameNum = 0;
        msg.action = chatResultReturn;
        formatMsgToJson_msgToClient(msg, buffer);
        send(userInfo_s->sockid, buffer, strlen(buffer), 0);
        freeAll(userInfo, pOption, message);
        return ;
    }

    elementType_LIST  temp;
    memset(temp.userInfo_c.name, 0, sizeof(temp.userInfo_c.name));
    strncpy(temp.userInfo_c.name, userInfo_s->userInfo_c.name, strlen(userInfo_s->userInfo_c.name));

    pthread_mutex_lock(&userList_mutex);
    Node_List *cur = list->head;
    for(; cur!=NULL; cur = cur->next){
        //printf("curName:%s\n", cur->item.userInfo_c.name);
        if((isEqual_itemName(cur->item, temp) || option) //私聊或群聊
            && (!isEqual_itemSockID(cur->item, clientNode.item))){//自己不对自己发信息
            msg.result = 1;
            msg.nameNum = 1;
            msg.names = (char (*)[40])malloc(sizeof(char[40]) *msg.nameNum);
            memset(msg.names[0], 0, sizeof(char[40]));
            memset(msg.message, 0, sizeof(msg.message));
            strncpy(msg.names[0], clientNode.item.userInfo_c.name, strlen(clientNode.item.userInfo_c.name));
            strncpy(msg.message, mess, strlen(mess));
            msg.action = messagePost;
            memset(buffer, 0, sizeof(buffer));
            formatMsgToJson_msgToClient(msg, buffer);
            send(cur->item.sockid, buffer, strlen(buffer), 0);
            free(msg.names);
            count++;
        }
    }
    pthread_mutex_unlock(&userList_mutex);

    if(count == 0){
        msg.result = 0;
        pthread_mutex_lock(&userList_mutex);
        msg.reasonCode = findNode(list, *userInfo_s, isEqual_itemName) == NULL ? 0 : 2; //聊天错误码：0， 用户不在线， 1，被禁言, 2对自己发消息
        pthread_mutex_unlock(&userList_mutex);
        msg.action = chatResultReturn;
        msg.nameNum = 0;
        formatMsgToJson_msgToClient(msg, buffer); 
        send(userInfo_s->sockid, buffer, strlen(buffer), 0);
    }
    if(count == 1){
        bzero(log, sizeof(log));
        sprintf(log, "用户%s向用户%s私发消息:%s\n", clientNode.item.userInfo_c.name, temp.userInfo_c.name, mess);
        printf(log);
        write(logfd, log, strlen(log));
    }
    if(count > 1){
        bzero(log, sizeof(log));
        sprintf(log, "用户%s群发消息:%s\n", clientNode.item.userInfo_c.name, mess);
        printf(log);
        write(logfd, log, strlen(log));
    }
       
    freeAll(userInfo, pOption, message);
}


void work_fileOperation(void * userList, void * userInfo, void * pOption, void * message){
    UserInfo_server *userInfo_s  = (UserInfo_server *)userInfo;
    List* list = (List*)userList;
    int option = *(int*)pOption;
    char *mess = (char*)message;

    msgToClient msg;
    char buffer[BUFFER_SIZE];

    if(option == 0){//客户端发来文件发送请求
        pthread_mutex_lock(&userList_mutex);
        
        Node_List *goalNode = findNode(list, *userInfo_s, isEqual_itemName);
        Node_List *clientNode = findNode(list, *userInfo_s,isEqual_itemSockID);
        UserInfo_server goalInfo, clientInfo;

        if(goalNode == NULL){//目标用户不在线
            msg.result = 0;
            msg.reasonCode = 0;
            msg.action = fileOperationResultReturn;
            pthread_mutex_unlock(&userList_mutex);
            memset(buffer, 0, sizeof(buffer));
            formatMsgToJson_msgToClient(msg, buffer);
            send(userInfo_s->sockid, buffer, strlen(buffer), 0);
            freeAll(userInfo, pOption, message); 
            return;
        }
        if(clientNode == NULL){
            printf("用户列表异常，操作发起用户不在线\n");
            pthread_mutex_unlock(&userList_mutex);
            freeAll(userInfo, pOption, message);
            return;
        }
        userInfoCopy(&goalInfo, goalNode->item);
        userInfoCopy(&clientInfo, clientNode->item);
        pthread_mutex_unlock(&userList_mutex);

        msg.action = sendFileRequest;
        msg.result = 1;
        msg.nameNum = 1;
        msg.reasonCode = 0;
        //给目标用户发松文件操作请求
        bzero(msg.message, sizeof(msg.message));
        strncpy(msg.message, mess , strlen(mess));
        msg.names = (char (*)[40])malloc(sizeof(char[40]) * msg.nameNum);
        bzero(msg.names[0], sizeof(char[40]));
        strncpy(msg.names[0], clientInfo.userInfo_c.name, strlen(clientInfo.userInfo_c.name));
        memset(buffer, 0, sizeof(buffer));
        formatMsgToJson_msgToClient(msg, buffer);
        send(goalInfo.sockid, buffer, strlen(buffer), 0);
        free(msg.names);
    }

    if(option == 1){//客户端拒绝了文件发送请求
        msg.result = 0;
        msg.reasonCode = 1;
        msg.action = fileOperationResultReturn;
        memset(buffer, 0, sizeof(buffer));
        formatMsgToJson_msgToClient(msg, buffer);

        pthread_mutex_lock(&userList_mutex);
        Node_List *goalNode = findNode(list, *userInfo_s, isEqual_itemName);

        if(goalNode != NULL ){
            send(goalNode->item.sockid, buffer, strlen(buffer), 0);
        }
        pthread_mutex_unlock(&userList_mutex);
    }

    if(option == 2){//接收端同意文件传输
        struct sockaddr_in clientAddr;
        socklen_t addrLen = sizeof(struct sockaddr_in);
        int len;
        msg.action = sendFileRequest;
        msg.result = 1;
        msg.nameNum = 1;
        msg.reasonCode = 1;
        //接收端ip端口号发给发送端...
        pthread_mutex_lock(&userList_mutex);
        UserInfo_server goalInfo;
        Node_List *goalNode = findNode(list, *userInfo_s, isEqual_itemName);
        if(goalNode == NULL){//发送端下线通知接受端
            msg.result = 0;
            msg.reasonCode = 2;
            msg.action = fileOperationResultReturn;
            pthread_mutex_unlock(&userList_mutex);
            memset(buffer, 0, sizeof(buffer));
            formatMsgToJson_msgToClient(msg, buffer);
            send(userInfo_s->sockid, buffer, strlen(buffer), 0);
            freeAll(userInfo, pOption, message);
            return ;
        }
        userInfoCopy(&goalInfo, goalNode->item);
        pthread_mutex_unlock(&userList_mutex);

        bzero(&clientAddr, sizeof(clientAddr));
        bzero(msg.message, sizeof(msg.message));
        bzero(buffer, sizeof(buffer));
        getpeername(userInfo_s->sockid,(struct sockaddr*)&clientAddr, &addrLen);
        len = strlen(inet_ntoa(clientAddr.sin_addr));
        strncpy(msg.message, inet_ntoa(clientAddr.sin_addr), len);
        msg.message[len] = ':';
        myItoA(clientAddr.sin_port, msg.message+len+1);
        //printf("addr: %s\n", msg.message);
        formatMsgToJson_msgToClient(msg, buffer);
        send(goalInfo.sockid, buffer, strlen(buffer), 0);
    }
    
    freeAll(userInfo, pOption, message);
}


void work_showOnline(void * userList, void * userInfo, void * pOption, void * message){
    UserInfo_server *userInfo_s  = (UserInfo_server *)userInfo;
    List* list = (List*)userList;
    int option = *(int*)pOption;
    char *mess = (char*)message;
    msgToClient msg;   
    char buffer[BUFFER_SIZE];
    msg.result = 1;
    pthread_mutex_lock(&userList_mutex);
    msg.nameNum = getLength(list);
    //printf("lenth:%d\n", msg.nameNum);
    msg.names = (char(*)[40])malloc(sizeof(char[40]) * msg.nameNum);
    Node_List *node = list->head;
    for(int i = 0; node != NULL; node = node->next, i++){
        memset(msg.names[i], 0, sizeof(char[40]));
       // printf("names[%d]:%s\n", i, node->item.userInfo_c.name);
        strncpy(msg.names[i], node->item.userInfo_c.name, strlen(node->item.userInfo_c.name));
    }
    pthread_mutex_unlock(&userList_mutex);

    msg.action = showUsers;
    memset(buffer, 0, sizeof(buffer));
    formatMsgToJson_msgToClient(msg, buffer);
    send(userInfo_s->sockid, buffer, strlen(buffer), 0);//用户过多buffer可能不够用......
    free(msg.names);
    freeAll(userInfo, pOption, message);
}


void work_superOperation(void * userList, void * userInfo, void * pOption, void * message){
    UserInfo_server *userInfo_s  = (UserInfo_server *)userInfo;
    List* list = (List*)userList;
    int option = *(int*)pOption;
    char *mess = (char*)message;
    msgToClient msg;   
    char buffer[BUFFER_SIZE];

    msg.action = superOperationResultReturn;
    msg.nameNum = 0;
    memset(buffer, 0, sizeof(buffer));

    pthread_mutex_lock(&userList_mutex);
    Node_List *tNode = findNode(list, *userInfo_s, isEqual_itemName);
    Node_List *temp = findNode(list, *userInfo_s, isEqual_itemSockID);
    UserInfo_server goalInfo, clientInfo;
    if(tNode == NULL){
        msg.result = 0;
        msg.reasonCode = 1;
        formatMsgToJson_msgToClient(msg, buffer);
        send(userInfo_s->sockid, buffer, strlen(buffer), 0);
        pthread_mutex_unlock(&userList_mutex);
        return;
    }
    if(temp == NULL){
        printf("用户列表异常，操作发起用户不在线\n");
        pthread_mutex_unlock(&userList_mutex);
        freeAll(userInfo, pOption, message);
        return;
    }
    userInfoCopy(&goalInfo, tNode->item);
    userInfoCopy(&clientInfo, temp->item);
    pthread_mutex_unlock(&userList_mutex);

    if(clientInfo.userType != root){
        msg.result = 0;
        msg.reasonCode = 0;
        formatMsgToJson_msgToClient(msg, buffer);
        send(userInfo_s->sockid, buffer, strlen(buffer), 0); 
        freeAll(userInfo, pOption, message);
        return;
    }

    msg.result = 1;
    msg.reasonCode = option + 3;
    memset(msg.message, 0, sizeof(msg.message));
    strncpy(msg.message, goalInfo.userInfo_c.name, strlen(goalInfo.userInfo_c.name));
    formatMsgToJson_msgToClient(msg, buffer);
    send(userInfo_s->sockid, buffer, strlen(buffer), 0);//发给操作者
    int sockid = goalInfo.sockid;//优化点，先链表数据保存下来在访问，不要对链表多次访问，不然要加锁解锁很多次，这里加锁解锁也不完善
    pthread_mutex_lock(&userList_mutex);
    switch(option){
        case 0: tNode->item.status = postBanend;    break; //禁言
        case 1: tNode->item.status = normal;   break;//解禁
        case 2: delNode(list, tNode->item, isEqual_itemID);  break;//踢人
        default : break;
    }
    pthread_mutex_unlock(&userList_mutex);

    memset(buffer, 0, sizeof(buffer));
    msg.reasonCode -= 3;
    formatMsgToJson_msgToClient(msg, buffer);
    send(sockid, buffer, strlen(buffer), 0);//发给被操作者
    freeAll(userInfo, pOption, message);
}


void perror_db(sqlite3* db, int errMsg, const char * msg){
    if(errMsg != SQLITE_OK){
        printf("%s : %s\n", msg, sqlite3_errmsg(db));
    }
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

void freeAll(void* userInfo, void*pOption, void*message){
    free(userInfo);
    free(pOption);
    free(message);
}

void userInfoCopy(UserInfo_server *dest, UserInfo_server src){
    dest->sockid = src.sockid;
    dest->status = src.status;
    dest->userType = src.userType;
    dest->userInfo_c.id = src.userInfo_c.id;
    bzero(dest->userInfo_c.name, sizeof(dest->userInfo_c.name));
    bzero(dest->userInfo_c.pwd, sizeof(dest->userInfo_c.pwd));
    strncpy(dest->userInfo_c.name, src.userInfo_c.name, strlen(src.userInfo_c.name));
    strncpy(dest->userInfo_c.pwd, src.userInfo_c.pwd, strlen(src.userInfo_c.pwd));
}