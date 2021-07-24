#ifndef __MYTYPE_H__
#define __MYTYPE_H__

#include "lib_use.h"

typedef enum ActionType_Client{
    Register, Login, Chat, broadCast, sendFile, showOnline, superOperation
}ActionType_Client;

typedef enum ActionType_Server{
    resultReturn, sendFileRequest, messagePost
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
    int result;
    int reasonCode;
    int nameNum;
    ActionType_Server action;
    char (*names)[40];
    char message[1024];
}msgToClient;


typedef struct UserInfo_server{
    struct sockaddr_in addr;
    UserInfo_Client userInfo;
    StatusType status;
    UserType user;
}UerInfo_server;

struct msgbuf{
    long mtype;
    char mtext[BUFFER_SIZE];
};

#endif