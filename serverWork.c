#include "serverWork.h"
#include "dataParse.h"

void work_register(void * userList, void * userInfo, void * pDataBase_mutex, void * pOption){
    UserInfo_server *userInfo_s  = (UserInfo_server *)userInfo;
    pthread_mutex_t *Database_mutex = (pthread_mutex_t *)pDataBase_mutex;
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
    
    printf("child: %d\n", userInfo_s->sockid); 
    int ret = sqlite3_open("local.db", &db);
    perror_db(db, ret, "open DataBase error");
    sprintf(sql, "insert into User(username, password) values('%s', '%s');",
    userInfo_s->userInfo_c.name, userInfo_s->userInfo_c.pwd);

    pthread_mutex_lock(Database_mutex);
    ret = sqlite3_exec(db, sql, NULL, NULL, &errMsg);
    perror_db(db, ret, "insert data base error");
    memset(sql, 0, sizeof(sql));
    sprintf(sql, "select (id) from User order by id desc limit 1");
    ret = sqlite3_get_table(db, sql, &table, &rowCount, &columnCount, &errMsg);
    perror_db(db, ret, "query  database error");
    userInfo_s->userInfo_c.id = atoi(table[1]);
    pthread_mutex_unlock(Database_mutex);

    msg.action = resultReturn;
    msg.result = 1;
    memset(msg.message, 0, sizeof(msg.message));
    myItoA(userInfo_s->userInfo_c.id, msg.message);
    memset(buffer, 0, sizeof(buffer));
    formatMsgToJson_msgToClient(msg, buffer);
    printf("send buffer: %s", buffer);
    send(userInfo_s->sockid, buffer, strlen(buffer), 0);//有问题
    sqlite3_free_table(table);
    sqlite3_close(db);
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