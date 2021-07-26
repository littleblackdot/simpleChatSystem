#ifndef __SERVERWORK_H__
#define __SERVERWORK_H__

#include "myType.h"
#include "lib_use.h"

void work_register(void *, void*, void*, void *);
void work_login(void *);
void work_chat(void *);
void work_broadCast(void *);
void work_fileSend(void *);
void work_showOnline(void *);
void work_superOperation(void *);


void perror_db(sqlite3* db, int errMsg, const char * msg);
void myItoA(const int , char *);



#endif