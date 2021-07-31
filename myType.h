#ifndef __MYTYPE_H__
#define __MYTYPE_H__

#include "lib_use.h"

#define FILENAMELEN 128
#define FILEPATHLEN 256
#define FILEDIVLEN 512 //每次传输的文件主题最大大小

typedef enum ActionType_Client{
    Register, Login, Chat, fileOperation, showOnline, superOperation
}ActionType_Client;

typedef enum ActionType_Server{
    chatResultReturn, showUsers, sendFileRequest, messagePost, superOperationResultReturn, fileOperationResultReturn
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

typedef struct FileInfo{
    char name[FILENAMELEN];
    int size;//文件大小
    int blockSize;//单个数据包包含文件主体大小
    int count;//文件分块数量
}FileInfo;


typedef struct PartOfFile{
    int offset;//文件分块的起始位置
    int seq;
    int realsize;
    char body[FILEDIVLEN];
}PartOfFile;


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