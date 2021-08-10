#ifndef __SERVERWORK_H__
#define __SERVERWORK_H__

#include "myType.h"
#include "lib_use.h"
#include "dataParse.h"
#include "List.h"

extern int logfd;


void work_register(void *, void*, void*, void*);
void work_login(void *, void*, void*, void*);
void work_chat(void *, void*, void*, void*);
void work_fileOperation(void *, void*, void*, void*);
void work_showOnline(void *, void*, void*, void*);
void work_superOperation(void *, void*, void*, void*);


void perror_db(sqlite3* db, int errMsg, const char * msg);
void myItoA(const int , char *);
void freeAll(void*, void*, void*);
void userInfoCopy(UserInfo_server *dest, UserInfo_server src);

#endif