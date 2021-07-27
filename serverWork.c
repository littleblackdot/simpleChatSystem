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
    char *errMsg;
    char **table;
    int columnCount;
    int rowCount;
    sqlite3 *db;

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
    char *errMsg;
    char **table;
    int columnCount;
    int rowCount;
    sqlite3 *db;

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
        free(userInfo);
        free(pOption);
        free(message);
        return ;
    }
    memset(userInfo_s->userInfo_c.name, 0, sizeof(userInfo_s->userInfo_c.name));
    strncpy(userInfo_s->userInfo_c.name, table[columnCount*1 + 1], strlen(table[columnCount*1 + 1]));
    userInfo_s->userType = (UserType)atoi(table[columnCount*1 + 3]);
    userInfo_s->status = normal;
    
    pthread_mutex_lock(&userList_mutex);
    insertNode(list, *userInfo_s);
    pthread_mutex_unlock(&userList_mutex);
    //displayList(list);
    msg.result = 1;
    memset(buffer, 0, sizeof(buffer));
    formatMsgToJson_msgToClient(msg, buffer);
    send(userInfo_s->sockid, buffer, strlen(buffer), 0);
    sqlite3_free_table(table);
    sqlite3_close(db);
    free(userInfo);
    free(pOption);
    free(message);
}


void work_chat(void * userList, void * userInfo, void * pOption, void * message){
    UserInfo_server *userInfo_s  = (UserInfo_server *)userInfo;
    List* list = (List*)userList;
    int option = *(int*)pOption;
    char *mess = (char*)message;
    int count = 0;
    msgToClient msg;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    if(userInfo_s->status == postBanend){
        msg.result = 0;
        msg.reasonCode = 1; //聊天错误码：0， 用户不在线， 1，被禁言
        msg.action = chatResultReturn;
        formatMsgToJson_msgToClient(msg, buffer);
        send(userInfo_s->sockid, buffer, strlen(buffer), 0);
        return ;
    }
    elementType_LIST  temp;
    memset(temp.userInfo_c.name, 0, sizeof(temp.userInfo_c.name));
    strncpy(temp.userInfo_c.name, userInfo_s->userInfo_c.name, strlen(userInfo_s->userInfo_c.name));

    pthread_mutex_lock(&userList_mutex);
    Node_List *clientNode = findNode(list, *userInfo_s, isEqual_itemID);
    Node_List *cur = list->head;
    for(; cur!=NULL; cur = cur->next){
        printf("curName:%s\n", cur->item.userInfo_c.name);
        if((isEqual_itemName(cur->item, temp) || option) //私聊或群聊
            && (!isEqual_itemSockID(cur->item, clientNode->item))){//自己不对自己发信息
            msg.result = 1;
            msg.nameNum = 1;
            msg.names = (char (*)[40])malloc(sizeof(char[40]) *msg.nameNum);
            memset(msg.names[0], 0, sizeof(char[40]));
            memset(msg.message, 0, sizeof(msg.message));
            strncpy(msg.names[0], clientNode->item.userInfo_c.name, strlen(clientNode->item.userInfo_c.name));
            strncpy(msg.message, mess, strlen(mess));
            msg.action = messagePost;
            memset(buffer, 0, sizeof(buffer));
            formatMsgToJson_msgToClient(msg, buffer);
            send(cur->item.sockid, buffer, strlen(buffer), 0);
            free(msg.names);
            count++;
            printf("1\n");
        }
    }
    pthread_mutex_unlock(&userList_mutex);
    
    if(count == 0){
        msg.result = 0;
        msg.reasonCode = 0; //聊天错误码：0， 用户不在线， 1，被禁言
        msg.action = chatResultReturn;
        formatMsgToJson_msgToClient(msg, buffer); 
        send(userInfo_s->sockid, buffer, strlen(buffer), 0);
    }
       
    free(userInfo);
    free(pOption);
    free(message);
}
void work_fileSend(void * userList, void * userInfo, void * pOption, void * message){
    UserInfo_server *userInfo_s  = (UserInfo_server *)userInfo;
    List* list = (List*)userList;
    int option = *(int*)pOption;
    char *mess = (char*)message;

    free(userInfo);
    free(pOption);
    free(message);
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
    free(userInfo);
    free(pOption);
    free(message);
}


void work_superOperation(void * userList, void * userInfo, void * pOption, void * message){
    UserInfo_server *userInfo_s  = (UserInfo_server *)userInfo;
    List* list = (List*)userList;
    int option = *(int*)pOption;
    char *mess = (char*)message;

    free(userInfo);
    free(pOption);
    free(message);
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