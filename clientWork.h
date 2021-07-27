#ifndef __CLIENTWORK_H__
#define __CLIENTWORK_H__

#include "lib_use.h"
#include "myType.h"
#include "dataParse.h"
extern int isLogin;

void work_register(int sockid);
void work_login(int sockid);
void work_chat(int sockid);
void work_sendFile(int sockid);
void work_showOnline(int sockid);
void work_superOperate(int sockid);

void messReceive(int sockid);

#endif