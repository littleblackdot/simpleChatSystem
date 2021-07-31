#ifndef __DATAPARSE_H__
#define __DATAPARSE_H__

#include "lib_use.h"

#include "cJSON.h"
#include "myType.h"




void formatMsgToJson_msgToServer(msgToServer msg, char * jsonContainer);
void parseJsonData_Server(msgToServer *pmsg, const char *jsonData);
void showMsg_msgToServer(msgToServer msg);

void formatMsgToJson_msgToClient(msgToClient msg, char * jsonContainer);
void parseJsonData_Client(msgToClient *pmsg, const char *jsonData);         
void showMsg_msgToClient(msgToClient msg);

void formatFileInfoToJson(FileInfo fileInfo, char * jsonContainer);
void parseJsonData_FileInfo(FileInfo *pfileInfo, const char *jsonData);


void formatPartOfFileInfoToJson(PartOfFile part, char *jsonContainer);
void parseJsonData_PartOfFile(PartOfFile *part, const char *jsonData);


#endif