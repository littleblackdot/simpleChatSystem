#ifndef __CLIENTWORK_H__
#define __CLIENTWORK_H__

#include "lib_use.h"
#include "myType.h"
#include "dataParse.h"
#define IPV4ADDRLEN 16

extern int isLogin;
extern int msgid;

void work_register(int sockid);
void work_login(int sockid);
void work_chat(int sockid);
void work_sendFile(int sockid);
void work_showOnline(int sockid);
void work_superOperate(int sockid);

void fileSendHelper(int sockid, const msgToClient *pmsg, const char *filepath);

void* checkFilesendRequest(void* sockid);
void receiveFile(int sockid, FileInfo *pfileInfo);
void sendFile(const char *goalAddr, const char* filePath);
void messReceive(int sockid);
void parseIPADDR(const char* addrString, struct sockaddr_in *addr);
int createfile(char *filename, int size);

#endif