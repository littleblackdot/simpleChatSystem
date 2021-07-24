#include "lib_use.h"
#include "myNet.h"
#include "dataParse.h"
#define EPOLLCAPACITY_INIT 2000
#define IPV4ADDRLEN 16
void sigHander(int sign);

int msgInit();

int main(){
    int sockid = socketInit_TCP(SERVER_LISTENPORT, SERVER_IPADDRESS);
    int pid; 
    int epid = epoll_create(EPOLLCAPACITY_INIT); 
    struct epoll_event ev;
    struct epoll_event all[EPOLLCAPACITY_INIT];

    ev.data.fd = sockid;
    ev.events = EPOLLIN;
    epoll_ctl(epid, EPOLL_CTL_ADD, sockid, &ev);
    int msgid = msgInit();
    pid = fork();
    if(pid < 0){
        perror("fork error");
        exit(1);
    }
    if(pid > 0){//父进程负责监听连接请求
        while(1){
            int count = epoll_wait(epid, all, sizeof(all)/sizeof(all[0]), -1);
            struct sockaddr_in clientAddr;
            socklen_t clAddrLen = sizeof(struct sockaddr_in);
            char ipaddr[IPV4ADDRLEN];
            short port;
            int sockid2;
            int offset = 0;
            for(int i = 0; i < count; i++){
                if(all[i].data.fd == sockid){
                    sockid2 = accept(sockid, (struct sockaddr*)&clientAddr, &clAddrLen);
                    //连接信息发送子进程......
                    struct msgbuf *pmsg = (struct msgbuf*) malloc(sizeof(struct msgbuf));
                    memset(pmsg->mtext, 0, sizeof(pmsg->mtext));
                    memset(ipaddr, 0, sizeof(ipaddr));
                    pmsg->mtype = 1;
                    offset = 0;
                    strncpy(ipaddr, inet_ntoa(clientAddr.sin_addr), IPV4ADDRLEN);
                    port = clientAddr.sin_port;

                    strncpy(pmsg->mtext, (char*)&sockid2, sizeof(sockid2));
                    offset += sizeof(sockid2);
                    strncpy(pmsg->mtext+offset, ipaddr, IPV4ADDRLEN);
                    offset += IPV4ADDRLEN;
                    strncpy(pmsg->mtext+offset, (char*)&port, sizeof(port));
                    msgsnd(msgid, pmsg, IPV4ADDRLEN+sizeof(port)+sizeof(sockid2), IPC_NOWAIT);
                    write(sockid2, "hello1\n", 7);               
                }
            }
        }    
    }else if(pid == 0){
        while(1){
            struct sockaddr_in clientAddr;
            int clAddrLen = sizeof(struct sockaddr_in);
            int sockid2;
            char ipaddr[IPV4ADDRLEN];
            short port;
            int offset = 0;
            struct msgbuf* pmsg = (struct msgbuf*)malloc(sizeof(struct msgbuf));

            memset(pmsg->mtext, 0, sizeof(pmsg->mtext));
            if(msgrcv(msgid, pmsg, sizeof(sockid2)+IPV4ADDRLEN+sizeof(port), 1, 0) < 0){
                perror("msgrcv error");
            }
            offset = 0; 
            strncpy((char*)&sockid2, pmsg->mtext, sizeof(sockid2));
            offset += sizeof(sockid2);
            strncpy((char*)&ipaddr, pmsg->mtext+offset, IPV4ADDRLEN);
            offset += IPV4ADDRLEN;
            strncpy((char*)&port, pmsg->mtext+offset, sizeof(port));
            write(sockid2, "hello2\n", 7);
        }
    }
    wait(NULL);
    return 0;
}

void sigHander(int sig){
    if(sig == SIGINT){
        wait(NULL);
        exit(1);
    }
}

int msgInit(){
    key_t key = ftok("test.c", 'a');
    if(key < 0 ){
        printf("create key failed\n");
        exit(1);
    }
    int shmid = msgget(key, IPC_CREAT|0655);
    if(shmid < 0){
        perror("msgget failed");
        exit(1);
    }
    return shmid;
}