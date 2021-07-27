#ifndef __MYTYPE_H__
#define __MYTYPE_H__

#include "lib_use.h"

typedef enum ActionType_Client{
    Register, Login, Chat, sendFile, showOnline, superOperation
}ActionType_Client;

typedef enum ActionType_Server{
    chatResultReturn, showUsers, sendFileRequest, messagePost, superOperationResultReturn
}ActionType_Server;

typedef enum StatusType{
    normal, postBanend
}StatusType;

typedef enum UserType{
    common, root
}UserType;

typedef struct USERINFO{
    int id;
    char name[40];
    char pwd[20];
}UserInfo_Client;

typedef struct MSGTOSERVER{
    ActionType_Client action;
    int actionOption;
    UserInfo_Client userInfo;
    char message[1024];
}msgToServer;


typedef struct MSGTOCLIENT{
    int result;//客户端业务请求处理结果，1 成功   0失敗
    int reasonCode;//原因码，不同情景意义不同
    int nameNum;
    ActionType_Server action;
    char (*names)[40];
    char message[1024];
}msgToClient;


typedef struct UserInfo_server{
    int sockid;
    UserInfo_Client userInfo_c;
    StatusType status;
    UserType userType;
}UserInfo_server;

struct msgbuf{
    long mtype;
    char mtext[BUFFER_SIZE];
};


typedef void (*taskFuncType)(void *, void*, void *, void *);
typedef struct Task{
	taskFuncType taskFunc;
	void *arg1;
    void *arg2;
    void *arg3;
    void *arg4;
}Task;

typedef void (*clientWorkFuncType)(int );

#endif