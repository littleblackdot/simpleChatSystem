#include "lib_use.h"
#include "myNet.h"
#include "dataParse.h"
#include "myThreadPool.h"
#include "serverWork.h"

#define EPOLLCAPACITY_INIT 2000
#define IPV4ADDRLEN 16
void sigHander(int sign);


int main(){
    int sockid = socketInit_TCP(SERVER_LISTENPORT, SERVER_IPADDRESS);
    int pid; 
    int epid = epoll_create(EPOLLCAPACITY_INIT); 
    struct epoll_event ev;
    struct epoll_event all[EPOLLCAPACITY_INIT];
    pthread_mutex_t Database_mutex;
    char buffer[BUFFER_SIZE];

    ev.data.fd = sockid;
    ev.events = EPOLLIN;
    epoll_ctl(epid, EPOLL_CTL_ADD, sockid, &ev);
    pThreadPool pthpool = threadPoolInit(10, 30);
    pthread_mutex_init(&Database_mutex, NULL);
    taskFuncType taskFuncs[7] = {work_register};
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
                int option;
                memset(buffer, 0, sizeof(buffer));
                count1 = recv(sockid2, buffer, sizeof(buffer), 0);
                if(count1 <= 0){
                    //用户的下线处理....
                    
                    continue;
                }
                msgToServer msg;
                parseJsonData_Server(&msg, buffer);
                UserInfo_server userInfo_s;
                userInfo_s.sockid = sockid2;
                memset(userInfo_s.userInfo_c.name, 0, sizeof(userInfo_s.userInfo_c.name));
                memset(userInfo_s.userInfo_c.pwd, 0, sizeof(userInfo_s.userInfo_c.pwd));
                strncpy(userInfo_s.userInfo_c.name, msg.userInfo.name, strlen(msg.userInfo.name));
                strncpy(userInfo_s.userInfo_c.pwd, msg.userInfo.pwd, strlen(msg.userInfo.pwd));
                userInfo_s.userType = common;
                userInfo_s.status = normal;
                option = msg.actionOption;
                Task task; 
                task.taskFunc = work_register;
                task.arg1 = NULL;
                task.arg2 = (void*)&userInfo_s;
                task.arg3 = &Database_mutex;
                task.arg4 = &option;
                addTask(pthpool, task);        
            }
        }  
    }
    return 0;
}
