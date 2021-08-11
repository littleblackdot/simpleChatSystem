#include "lib_use.h"
#include "myNet.h"
#include "myType.h"
#include "dataParse.h"
#include "myThreadPool.h"
#include "serverWork.h"
#include "List.h"

#define EPOLLCAPACITY_INIT 2000
#define IPV4ADDRLEN 16
void sigHander(int sign);

int sockid;
int logfd;
pThreadPool pthpool;

pthread_mutex_t dataBase_mutex;
pthread_mutex_t userList_mutex; 

int main(){
    sockid = socketInit_TCP(SERVER_LISTENPORT, SERVER_IPADDRESS);
    int pid; 
    int epid = epoll_create(EPOLLCAPACITY_INIT);
    List *list = createList();
    struct epoll_event ev;
    struct epoll_event all[EPOLLCAPACITY_INIT];
    char buffer[BUFFER_SIZE];
    char log[BUFFER_SIZE];
    logfd = open("log_server.txt", O_RDWR|O_CREAT|O_APPEND, 0655);
    ev.data.fd = sockid;
    ev.events = EPOLLIN;
    epoll_ctl(epid, EPOLL_CTL_ADD, sockid, &ev);
    pthpool = threadPoolInit(10, 30);
    pthread_mutex_init(&dataBase_mutex, NULL);
    pthread_mutex_init(&userList_mutex, NULL);
    taskFuncType taskFuncs[6] = {work_register, work_login, work_chat, work_fileOperation, work_showOnline, work_superOperation, };
    while(1){
        int count = epoll_wait(epid, all, sizeof(all)/sizeof(all[0]), -1);
        struct sockaddr_in clientAddr;
        socklen_t clAddrLen = sizeof(struct sockaddr_in);
        int offset = 0;
        for(int i = 0; i < count; i++){
            if(all[i].data.fd == sockid){
                int sockid2 = accept(sockid, (struct sockaddr*)&clientAddr, &clAddrLen);
                struct epoll_event temp;
                temp.data.fd = sockid2;
                temp.events = EPOLLIN;
                epoll_ctl(epid, EPOLL_CTL_ADD, sockid2, &temp);
            }else{
                int sockid2 = all[i].data.fd;
                int count1;
                memset(buffer, 0, sizeof(buffer));
                count1 = recv(sockid2, buffer, sizeof(buffer), 0);
                if(count1 <= 0){
                    //用户的下线处理....
                    struct epoll_event t;
                    UserInfo_server temp ;
                    Node_List *offLineNode;
                    temp.sockid = sockid2;
                    pthread_mutex_lock(&userList_mutex);
                    offLineNode = findNode(list, temp, isEqual_itemSockID);
                    if(offLineNode != NULL){
                        //showItem(offLineNode->item);
                        sprintf(log, "用户下线 id :%d\n", offLineNode->item.userInfo_c.id);
                        printf(log);
                        write(logfd, log, strlen(log));
                        delNode(list, offLineNode->item, isEqual_itemID);
                        bzero(log, sizeof(log));
                        
                    }
                    pthread_mutex_unlock(&userList_mutex); 
                    shutdown(sockid2,SHUT_RDWR);
                    t.data.fd = sockid2;
                    t.events = EPOLLIN;
                    epoll_ctl(epid, EPOLL_CTL_DEL, sockid2, &t);
                    continue;
                }
                int *option = (int*)malloc(sizeof(int));
                char *message = (char*)malloc(sizeof(char) * BUFFER_SIZE);
                UserInfo_server *userInfo_s = (UserInfo_server *)malloc(sizeof(UserInfo_server));
                msgToServer msg;
                //printf("buffer:%s\n", buffer);
                parseJsonData_Server(&msg, buffer);
               
                userInfo_s->sockid = sockid2;
                userInfo_s->userInfo_c.id = msg.userInfo.id;
                memset(userInfo_s->userInfo_c.name, 0, sizeof(userInfo_s->userInfo_c.name));
                memset(userInfo_s->userInfo_c.pwd, 0, sizeof(userInfo_s->userInfo_c.pwd));
                memset(message, 0, BUFFER_SIZE);
                strncpy(userInfo_s->userInfo_c.name, msg.userInfo.name, strlen(msg.userInfo.name));
                strncpy(userInfo_s->userInfo_c.pwd, msg.userInfo.pwd, strlen(msg.userInfo.pwd));
                strncpy(message, msg.message, strlen(msg.message));
                *option = msg.actionOption;
                Task task; 
                task.taskFunc = taskFuncs[msg.action];
                task.arg1 = (void *)list;
                task.arg2 = (void *)userInfo_s;
                task.arg3 = (void *)option;
                task.arg4 = (void *)message;
                addTask(pthpool, task);        
            }
        }  
    }
    return 0;
}

void singHander(int sign){
    if(sign == SIGINT){
        pthread_mutex_destroy(&dataBase_mutex);
        pthread_mutex_destroy(&userList_mutex);
        threadPoolDestroy(&pthpool);
        shutdown(sockid, SHUT_RDWR);
        close(logfd);
        exit(1);
    }

}
