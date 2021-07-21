#ifndef __DATAPARSE_H__
#define __DATAPARSE_H__

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "cJSON.h"

typedef enum ActionType_Client{
    Register, Login, Chat, broadCast, sendFile, showOnline, superOperation
}ActionType_Client;

typedef enum ActionType_Server{
    resultReturn, sendFileRequest, messagePost
}ActionType_Server;

typedef struct USERINFO{
    int id;
    char name[40];
    char pwd[20];
}UserInfo;


typedef struct MSGTOSERVER{
    ActionType_Client action;
    int actionOption;
    UserInfo userInfo;
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




void formatMsgToJson_msgToServer(const msgToServer msg, char * jsonContainer);
void parseJsonData_Server(msgToServer *pmsg, const char *jsonData);
void showMsg_msgToServer(const msgToServer msg);

void formatMsgToJson_msgToClient(const msgToClient msg, char * jsonContainer);
void parseJsonData_Client(msgToClient *pmsg, const char *jsonData);
void showMsg_msgToClient(const msgToClient msg);


#endif